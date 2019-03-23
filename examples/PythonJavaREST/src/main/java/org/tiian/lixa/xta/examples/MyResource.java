// This is a derivative work of
// https://medium.com/dekses/how-to-dockerize-java-restful-api-application-7aed70ef3c3a
// https://github.com/dekses/jersey-docker-demo
//
// This example is not intended as blueprint to be used for a real application,
// but only as a technological demonstration of an ACID 2 phase commit
// distributed transaction in a REST contest

package org.tiian.lixa.xta.examples;

import javax.ws.rs.GET;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Consumes;
import javax.ws.rs.Produces;
import javax.ws.rs.FormParam;
import javax.ws.rs.core.MediaType;

// import XTA package from LIXA project
import org.tiian.lixa.xta.*;
// import SQL
import java.sql.Connection;
import java.sql.Statement;
// import Java XA
import javax.sql.XAConnection;
import javax.transaction.xa.XAResource;
import javax.transaction.xa.XAException;
// import PostgreSQL package for XA Data Source
import org.postgresql.xa.PGXADataSource;
// import Java io
import java.io.*;

/**
 * Root resource (exposed at "myresource" path)
 */
@Path("myresource")
public class MyResource {
    // XTA objects
    TransactionManager tm;
    Transaction tx;
    // XA Resource for PostgreSQL
    PGXADataSource xads;
    XAConnection xac;
    XAResource xar;
    Connection conn;
    Statement stmt;
    String sqlStatement;
    /**
     * Method handling HTTP GET requests. The returned object will be sent
     * to the client as "text/plain" media type.
     * @return String that will be returned as a text/plain response.
     */
    @GET
    @Produces(MediaType.TEXT_PLAIN)
    public String getIt() {
        return "Service up and running, use POST to do something useful!";
    }

    @POST
    @Consumes(MediaType.APPLICATION_FORM_URLENCODED)
    @Produces(MediaType.TEXT_PLAIN)
    public String postIt(@FormParam("xid") String xid,
			 @FormParam("oper") String oper) {
	try {
	    System.out.println("\n***** REST service called: xid='" + xid +
			       "', oper='" + oper + "' *****");
	    // build the SQL statement that must be executed
	    if (oper.equals("insert"))
		sqlStatement = "INSERT INTO authors VALUES(1804, 'Hawthorne', 'Nathaniel')";
	    else if (oper.equals("delete"))
		sqlStatement = "DELETE FROM authors WHERE id=1804";
	    else throw new Exception("Specified operation >" + oper + "< does not exist");
	    //
	    // A bit of scaffolding for PostgreSQL:
	    //
	    // 1. create an XA Data Source
	    xads = new PGXADataSource();
	    // 2. set connection parameters (one property at a time)
	    xads.setServerName(System.getenv("PQSERVER"));
	    xads.setDatabaseName("lixa");
	    xads.setUser("lixa");
	    xads.setPassword("passw0rd");
	    // 3. get an XA Connection from the XA Data Source
	    xac = xads.getXAConnection();
	    // 4. get an XA Resource from the XA Connection
	    xar = xac.getXAResource();
	    // 5. get an SQL Connection from the XA Connection
	    conn = xac.getConnection();
	    //
	    // XTA cod
	    //
	    // Create a mew XTA Transaction Manager
	    tm = new TransactionManager();
	    // Create a new XA global transaction using the Transaction
	    // Manager as a factory
	    tx = tm.createTransaction();
	    // Enlist PostgreSQL resource to transaction
	    tx.enlistResource(xar, "PostgreSQL",
			      System.getenv("PQSERVER") + ";u=lixa;db=lixa");
	    // create a new branch in the same global transaction
	    tx.branch(xid);
	    System.out.println("Created a subordinate branch " +
			       "with XID '" + tx.getXid().toString() + "'");
	    //
	    // Create and Execute a JDBC statement for PostgreSQL
	    //
	    System.out.println("PostgreSQL: executing SQL statement >" +
			       sqlStatement + "<");
	    // create a Statement object
	    stmt = conn.createStatement();
	    // Execute the statement
	    stmt.executeUpdate(sqlStatement);
	    // close the statement
	    stmt.close();
	    // perform first phase of commit (PREPARE ONLY)
	    System.out.println("Executing first phase of commit (prepare)");
	    tx.commit(true);
	    // start a backgroud thread: control must be returned to the client
	    // but finalization must go on in parallel
	    new Thread(new Runnable() {
		    @Override
		    public void run() {
			try {
			    // perform second phase of commit
			    System.out.println("Executing second phase of commit");
			    tx.commit(false);
			    // Close Statement, SQL Connection and XA
			    // Connection for PostgreSQL
			    stmt.close();
			    conn.close();
			    xac.close();
			} catch  (XtaException e) {
			    System.err.println("XtaException: LIXA ReturnCode=" +
					       e.getReturnCode() + " ('" +
					       e.getMessage() + "')");
			    e.printStackTrace();
			} catch (Exception e) {
			    e.printStackTrace();
			}
		    }
		}).start();
	} catch (XtaException e) {
	    System.err.println("XtaException: LIXA ReturnCode=" +
			       e.getReturnCode() + " ('" +
			       e.getMessage() + "')");
	    e.printStackTrace();
	} catch (Exception e) {
	    e.printStackTrace();
	}
	System.out.println("Returning 'PREPARED' to the client");
        return "PREPARED";
    }

}
