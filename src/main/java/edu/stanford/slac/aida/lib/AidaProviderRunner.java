/*
 * @file
 * Channel Provider Runner.
 */
package edu.stanford.slac.aida.lib;

import org.epics.pvaccess.PVAException;

import java.util.logging.Logger;

import static edu.stanford.slac.aida.impl.AidaService.elapsedTime;

/**
 * Channel Provider Runner.
 * This class can run a Channel Provider so that it can service requests.
 * It is called by {@link edu.stanford.slac.aida.impl.AidaService#main}.
 */
public class AidaProviderRunner {
    /**
     * Logger to log info
     */
    private static final Logger logger = Logger.getLogger(AidaProviderRunner.class.getName());

    /**
     * Run the given AIDA-PVA Channel Provider
     * @param aidaChannelProvider the given AIDA-PVA Channel Provider
     */
    public static void run(ChannelProvider aidaChannelProvider) {
        // Create new RPCServer
        AidaRPCServer server = new AidaRPCServer(aidaChannelProvider);

        // Create new Service for handling requests on the server
        // pass it an AidaChannelProvider which implements request() method
        AidaRPCService service = new AidaRPCService(aidaChannelProvider);

        // Register each channel hosted by this server, with the service
        server.registerServices(service);

        // Run the server to start servicing requests
        try {
            server.printInfo();   // Print RPC service information
            logger.info("Server Ready: " + elapsedTime());
            server.run(0);
        } catch (PVAException e) {
            e.printStackTrace();
        }
    }
}
