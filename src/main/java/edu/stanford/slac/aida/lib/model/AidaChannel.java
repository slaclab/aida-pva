/*
 * @file
 * This class encapsulates an AidaChannel.
 */
package edu.stanford.slac.aida.lib.model;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import lombok.NonNull;

/**
 * This class encapsulates an {@link AidaChannel}.
 * <p>
 * It is used to represent a single channel that this Channel Provider will support.
 * <p>
 * @note
 * It uses the `@Data` annotation to provide all the getters and setters,
 * a constructor with all required arguments,
 * and an equals(), hashcode() and toString()  method.
 * @note
 * It also uses the `@NoArgsConstructor` annotation to provide a constructor
 * with no arguments.
 * @note
 * And the `@AllArgsConstructor` annotation to provide a constructor
 * with all arguments.
 */
@Data
@NoArgsConstructor
@AllArgsConstructor
public class AidaChannel {
    /**
     * The name of the channel.  This can be a direct verbatim match or
     * a WildcardMatcher string - see documentation in code
     * for what is supported.
     * <p>
     * Note that at the time of writing there is a bug in WildcardMatcher
     * such that if you use more than one '*' in the string undefined results occur.
     */
    private @NonNull String channel;

    /**
     * This is the {@link AidaChannelOperationConfig} for all get requests to this channel
     */
    private AidaChannelOperationConfig getterConfig;

    /**
     * This is the {@link AidaChannelOperationConfig} for all set requests to this channel
     */
    private AidaChannelOperationConfig setterConfig;
}
