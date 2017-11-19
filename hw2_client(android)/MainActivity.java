package com.example.phone_client;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;

import android.support.v7.app.ActionBarActivity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

class hw2_client extends AsyncTask<String, Integer, Long> {		//network thread can't run in main thread.
	String serverName;
  	int port;
	hw2_client()
	{
		serverName = new String("192.168.0.4");
		//serverName = new String("10.16.17.165");

		port = 7777;
	}
	@Override
	protected Long doInBackground(String... params) {		//run in background method.
		while(true){
			try
		      {
	         System.out.println("Connecting to " + serverName
	                             + " on port " + port);		//debugging message.
	         Socket client = new Socket(serverName, port);	//socket connect address.
	         System.out.println("Just connected to "
	                      + client.getRemoteSocketAddress());
	         OutputStream outToServer = client.getOutputStream();
	         DataOutputStream out =
	                       new DataOutputStream(outToServer);	// make stream.

	         out.writeUTF("phone from "
	                      + client.getLocalSocketAddress());	//send data phone from 'my ip'

	         client.close();
		Thread.sleep(800);
	      }catch(Exception e)
	      {
	         e.printStackTrace();
	      }
		}
	}

}

public class MainActivity extends ActionBarActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        new hw2_client().execute();
  
   
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}

