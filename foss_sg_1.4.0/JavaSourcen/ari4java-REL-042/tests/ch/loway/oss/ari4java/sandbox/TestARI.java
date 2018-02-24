package ch.loway.oss.ari4java.sandbox;

import java.util.List;

import ch.loway.oss.ari4java.ARI;
import ch.loway.oss.ari4java.AriVersion;
import ch.loway.oss.ari4java.generated.ActionApplications;
import ch.loway.oss.ari4java.generated.ActionAsterisk;
import ch.loway.oss.ari4java.generated.ActionEvents;
import ch.loway.oss.ari4java.generated.Application;
import ch.loway.oss.ari4java.generated.AsteriskInfo;
import ch.loway.oss.ari4java.generated.Message;
import ch.loway.oss.ari4java.generated.Variable;
import ch.loway.oss.ari4java.tools.AriCallback;
import ch.loway.oss.ari4java.tools.RestException;
import ch.loway.oss.ari4java.tools.http.NettyHttpClient;

public class TestARI {

    public static void main(String[] args) {
		ARI ari = new ARI();
		NettyHttpClient hc = new NettyHttpClient();
		try {
			hc.initialize("http://192.168.0.194:8088/", "admin", "admin");
			ari.setHttpClient(hc);
			ari.setWsClient(hc);
			ari.setVersion(AriVersion.ARI_0_0_1);
			ActionApplications ac = ari.getActionImpl(ActionApplications.class);
			List<? extends Application> alist = ac.list();
			for (Application app : alist) {
				System.out.println(app.getName());
			}
			ActionAsterisk aa = ari.getActionImpl(ActionAsterisk.class);
			AsteriskInfo ai = aa.getInfo("");
			System.out.println(ai.getSystem().getEntity_id());
			// Let's try async
			aa.getInfo("", new AriCallback<AsteriskInfo>() {
				@Override
				public void onSuccess(AsteriskInfo result) {
					System.out.println(result.getSystem().getEntity_id());
				}
				@Override
				public void onFailure(RestException e) {
					e.printStackTrace();
				}
			});
			aa.getGlobalVar("AMPMGRPASS", new AriCallback<Variable>() {
				@Override
				public void onSuccess(Variable result) {
					System.out.println(result.getValue());
				}
				@Override
				public void onFailure(RestException e) {
					e.printStackTrace();
				}
			});
			aa.setGlobalVar("WHATUP", "Hoo", new AriCallback<Void>() {
				@Override
				public void onSuccess(Void result) {
					System.out.println("Done");
				}
				@Override
				public void onFailure(RestException e) {
					e.printStackTrace();
				}
			});
			aa.getGlobalVar("WHATUP", new AriCallback<Variable>() {
				@Override
				public void onSuccess(Variable result) {
					System.out.println(result.getValue());
				}
				@Override
				public void onFailure(RestException e) {
					e.printStackTrace();
				}
			});
			System.out.println("Waiting for response...");
			ActionEvents ae = ari.getActionImpl(ActionEvents.class);
			ae.eventWebsocket("hello,goodbye", new AriCallback<Message>() {
				@Override
				public void onSuccess(Message result) {
					System.out.println("ws="+result);
				}
				
				@Override
				public void onFailure(RestException e) {
					e.printStackTrace();
				}
			});
			Thread.sleep(5000); // Allow wheels to turn before applying brakes
			ari.closeAction(ae);
			Thread.sleep(5000); 
			hc.destroy();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
    
}
