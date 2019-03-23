// This is a derivative work of
// https://medium.com/dekses/how-to-dockerize-java-restful-api-application-7aed70ef3c3a
// https://github.com/dekses/jersey-docker-demo
//
// This example is not intended as blueprint to be used for a real application,
// but only as a technological demonstration of an ACID 2 phase commit
// distributed transaction in a REST contest

package org.tiian.lixa.xta.examples;

import org.glassfish.grizzly.http.server.HttpServer;
import org.glassfish.jersey.grizzly2.httpserver.GrizzlyHttpServerFactory;
import org.glassfish.jersey.server.ResourceConfig;

import java.io.IOException;
import java.net.URI;

/**
 * Main class.
 */
public class Main {
    // Base URI the Grizzly HTTP server will listen on
    public static final String BASE_URI = "http://0.0.0.0:8080/xta/";

    /**
     * Starts Grizzly HTTP server exposing JAX-RS resources defined in this
     * application.
     * @return Grizzly HTTP server.
     */
    public static HttpServer startServer() {
        // create a resource config that scans for JAX-RS resources and
	// providers in org.tiian.lixa.xta.examples package
        final ResourceConfig rc =
	    new ResourceConfig().packages("org.tiian.lixa.xta.examples");

        // create and start a new instance of grizzly http server
        // exposing the Jersey application at BASE_URI
        return GrizzlyHttpServerFactory.
	    createHttpServer(URI.create(BASE_URI), rc);
    }

    /**
     * Main method.
     * @param args
     * @throws IOException
     */
    public static void main(String[] args) throws IOException {
        final HttpServer server = startServer();
        System.out.println(String.format("Jersey app started with WADL available at " +
					 "%sapplication.wadl\nHit enter to stop it...", BASE_URI));
        System.in.read();
        server.stop();
    }
}

