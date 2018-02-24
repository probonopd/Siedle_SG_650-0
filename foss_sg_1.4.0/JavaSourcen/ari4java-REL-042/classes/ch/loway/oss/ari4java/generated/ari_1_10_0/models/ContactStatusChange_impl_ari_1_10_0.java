package ch.loway.oss.ari4java.generated.ari_1_10_0.models;

// ----------------------------------------------------
//      THIS CLASS WAS GENERATED AUTOMATICALLY         
//               PLEASE DO NOT EDIT                    
//    Generated on: Wed Aug 31 18:05:11 CEST 2016
// ----------------------------------------------------

import ch.loway.oss.ari4java.generated.*;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import java.util.Date;
import java.util.List;
import java.util.Map;

/**********************************************************
 * The state of a contact on an endpoint has changed.
 * 
 * Defined in file: events.json
 * Generated by: Model
 *********************************************************/

public class ContactStatusChange_impl_ari_1_10_0 extends Event_impl_ari_1_10_0 implements ContactStatusChange, java.io.Serializable {
private static final long serialVersionUID = 1L;
  /**    */
  private ContactInfo contact_info;
 public ContactInfo getContact_info() {
   return contact_info;
 }

 @JsonDeserialize( as=ContactInfo_impl_ari_1_10_0.class )
 public void setContact_info(ContactInfo val ) {
   contact_info = val;
 }

  /**    */
  private Endpoint endpoint;
 public Endpoint getEndpoint() {
   return endpoint;
 }

 @JsonDeserialize( as=Endpoint_impl_ari_1_10_0.class )
 public void setEndpoint(Endpoint val ) {
   endpoint = val;
 }

/** No missing signatures from interface */
}

