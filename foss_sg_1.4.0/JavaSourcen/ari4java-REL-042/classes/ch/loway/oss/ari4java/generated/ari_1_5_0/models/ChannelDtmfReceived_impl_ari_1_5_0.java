package ch.loway.oss.ari4java.generated.ari_1_5_0.models;

// ----------------------------------------------------
//      THIS CLASS WAS GENERATED AUTOMATICALLY         
//               PLEASE DO NOT EDIT                    
//    Generated on: Wed Aug 31 18:05:10 CEST 2016
// ----------------------------------------------------

import ch.loway.oss.ari4java.generated.*;
import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import java.util.Date;
import java.util.List;
import java.util.Map;

/**********************************************************
 * DTMF received on a channel.
 * 
 * This event is sent when the DTMF ends. There is no notification about the start of DTMF
 * 
 * Defined in file: events.json
 * Generated by: Model
 *********************************************************/

public class ChannelDtmfReceived_impl_ari_1_5_0 extends Event_impl_ari_1_5_0 implements ChannelDtmfReceived, java.io.Serializable {
private static final long serialVersionUID = 1L;
  /**  The channel on which DTMF was received  */
  private Channel channel;
 public Channel getChannel() {
   return channel;
 }

 @JsonDeserialize( as=Channel_impl_ari_1_5_0.class )
 public void setChannel(Channel val ) {
   channel = val;
 }

  /**  DTMF digit received (0-9, A-E, # or *)  */
  private String digit;
 public String getDigit() {
   return digit;
 }

 @JsonDeserialize( as=String.class )
 public void setDigit(String val ) {
   digit = val;
 }

  /**  Number of milliseconds DTMF was received  */
  private int duration_ms;
 public int getDuration_ms() {
   return duration_ms;
 }

 @JsonDeserialize( as=int.class )
 public void setDuration_ms(int val ) {
   duration_ms = val;
 }

/** No missing signatures from interface */
}
