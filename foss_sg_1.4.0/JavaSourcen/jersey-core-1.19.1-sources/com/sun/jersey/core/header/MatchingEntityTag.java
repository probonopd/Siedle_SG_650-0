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

package com.sun.jersey.core.header;

import com.sun.jersey.core.header.reader.HttpHeaderReader;
import com.sun.jersey.core.header.reader.HttpHeaderReader.Event;
import com.sun.jersey.impl.ImplMessages;
import java.text.ParseException;
import java.util.Collections;
import java.util.Set;
import javax.ws.rs.core.EntityTag;

/**
 * A matching entity tag.
 * <p>
 * Note that this type and it's super type cannot be used to create request
 * header values for <code>If-Match</code> and <code>If-None-Match</code>
 * of the form <code>If-Match: *</code> or <code>If-None-Match: *</code> as
 * <code>*</code> is not a valid entity tag.
 *
 * @author Paul.Sandoz@Sun.Com
 */
public class MatchingEntityTag extends EntityTag {

    /**
     * An empty set that corresponds to <code>If-Match: *</code> or
     * <code>If-None-Match: *</code>.
     */
    public static final Set<MatchingEntityTag> ANY_MATCH = Collections.emptySet();

    /**
     * Create new strongly validating entity tag.
     *
     * @param value ETag header value.
     */
    public MatchingEntityTag(String value) {
        super(value, false);
    }

    /**
     * Create new matching entity tag.
     *
     * @param value ETag header value.
     * @param weak should be set to false, if strong validation is required,
     *            otherwise should be set to true.
     */
    public MatchingEntityTag(String value, boolean weak) {
        super(value, weak);
    }

    /**
     * Create new matching entity tag out of provided header reader.
     *
     * @param reader HTTP header content reader.
     * @return a new matching entity tag.
     * @throws ParseException in case the header could not be parsed.
     */
    public static MatchingEntityTag valueOf(HttpHeaderReader reader) throws ParseException {

        final String originalHeader = reader.getRemainder();

        Event e = reader.next(false);
        if (e == Event.QuotedString) {
            return new MatchingEntityTag(reader.getEventValue());
        } else if (e == Event.Token) {
            String v = reader.getEventValue();
            if (v.equals("W")) {
                reader.nextSeparator('/');
                return new MatchingEntityTag(reader.nextQuotedString(), true);
            }
        }

        throw new ParseException(ImplMessages.ERROR_PARSING_ENTITY_TAG(originalHeader), reader.getIndex());
    }
}