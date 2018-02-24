/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 * Copyright (c) 2010-2013 Oracle and/or its affiliates. All rights reserved.
 *
 * The contents of this file are subject to the terms of either the GNU
 * General Public License Version 2 only ("GPL") or the Common Development
 * and Distribution License("CDDL") (collectively, the "License").  You
 * may not use this file except in compliance with the License.  You can
 * obtain a copy of the License at
 * http://glassfish.java.net/public/CDDL+GPL_1_1.html
 * or packager/legal/LICENSE.txt.  See the License for the specific
 * language governing permissions and limitations under the License.
 *
 * When distributing the software, include this License Header Notice in each
 * file and include the License file at packager/legal/LICENSE.txt.
 *
 * GPL Classpath Exception:
 * Oracle designates this particular file as subject to the "Classpath"
 * exception as provided by Oracle in the GPL Version 2 section of the License
 * file that accompanied this code.
 *
 * Modifications:
 * If applicable, add the following below the License Header, with the fields
 * enclosed by brackets [] replaced by your own identifying information:
 * "Portions Copyright [year] [name of copyright owner]"
 *
 * Contributor(s):
 * If you wish your version of this file to be governed by only the CDDL or
 * only the GPL Version 2, indicate your decision by adding "[Contributor]
 * elects to include this software in this distribution under the [CDDL or GPL
 * Version 2] license."  If you don't indicate a single choice of license, a
 * recipient has the option to distribute your version of this file under
 * either the CDDL, the GPL Version 2 or to extend the choice of license to
 * its licensees as provided above.  However, if you add GPL Version 2 code
 * and therefore, elected the GPL Version 2 license, then the option applies
 * only if the new code is made subject to such option by the copyright
 * holder.
 */
package com.sun.jersey.api.client;


import javax.ws.rs.core.Response;
import javax.ws.rs.core.Response.StatusType;

/**
 * Factory for producing custom JAX-RS {@link javax.ws.rs.core.Response.StatusType response status type}
 * instances.
 *
 * @author Paul Sandoz
 */
public final class Statuses {

    private static final class StatusImpl implements StatusType {

        private int code;
        private String reason;
        private Response.Status.Family family;

        private StatusImpl(int code, String reason) {
            this.code = code;
            this.reason = reason;
            this.family = ClientResponse.Status.getFamilyByStatusCode(code);
        }

        @Override
        public int getStatusCode() {
            return code;
        }

        @Override
        public String getReasonPhrase() {
            return reason;
        }

        @Override
        public String toString() {
            return reason;
        }

        @Override
        public Response.Status.Family getFamily() {
            return family;
        }
    }

    /**
     * Create a new {@link StatusType status type} from the given code. Status type if firstly
     * searched in {@link Response.Status} and if none is found, then a custom one is created and
     * initialized with status code (with empty reason phrase).
     *
     * @param code Status code.
     * @return Non-null value of status type.
     */
    public static StatusType from(int code) {
        StatusType result = ClientResponse.Status.fromStatusCode(code);
        return (result != null) ? result : new StatusImpl(code, "");
    }

    /**
     * Creates a new custom {@link StatusType status type} initialized from the
     * status code and reason phrase.
     *
     * @param code Status code.
     * @param reason Reason phrase.
     * @return Non-null value of status type.
     */
    public static StatusType from(int code, String reason) {
        return new StatusImpl(code, reason);
    }


    /**
     * Prevents instantiation.
     */
    private Statuses() {
    }
}
