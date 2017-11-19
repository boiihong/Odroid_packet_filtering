package com.example.hw2_server;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import android.support.v7.app.ActionBarActivity;
import android.text.method.ScrollingMovementMethod;
import android.os.Bundle;
import android.os.Handler;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;


public class MainActivity extends ActionBarActivity implements OnClickListener {

	public ServerSocket hw2_server;	//server
	public TextView hw2_tv; 	// monitoring incomming packet
	public Button filter;	//button for filter on / off
	public Button filter2;
	public Button filter3;
	public File proc_f;
	public FileWriter fw;
	public BufferedWriter bfw;
	public String str;		//string for procfs write
	public String str2;
	public String str3;
	public Thread hw_Runnable;
	public Handler hw_handler;	//UI thread ...
	public String server_read;
	public Socket server;
	public File hw_log;
	public BufferedWriter hw_fw;
	public void onClick(View v)  {
		int id = v.getId();
		
		if(id == R.id.my_button1)
		{
			if(new File("proc/hw2_proc").exists()){
			 try {
				proc_f = new File("/proc/hw2_proc");	//open file
				fw = new FileWriter(proc_f);	//write stream is opened 
				bfw = new BufferedWriter(fw);	
				bfw.write(str);					//write "hw2_desktop" to hw2_proc
				bfw.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		}
		else if(id == R.id.my_button2)
		{
			if(new File("proc/hw2_proc").exists()){
				 try {
					proc_f = new File("/proc/hw2_proc");
					fw = new FileWriter(proc_f);
					bfw = new BufferedWriter(fw);
					bfw.write(str2);
					bfw.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
		else if(id == R.id.my_button3)
		{
			if(new File("proc/hw2_proc").exists()){
				 try {
					proc_f = new File("/proc/hw2_proc");
					fw = new FileWriter(proc_f);
					bfw = new BufferedWriter(fw);
					bfw.write(str3);
					bfw.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        hw2_tv = (TextView)findViewById(R.id.my_editText1);
        hw2_tv.setMaxLines(300);					// max line is 300
        hw2_tv.setVerticalScrollBarEnabled(true);	// scroll option
        hw2_tv.setMovementMethod(new ScrollingMovementMethod());
        hw2_tv.setText("start\n");
       
        filter = (Button)findViewById(R.id.my_button1);
        filter.setOnClickListener((OnClickListener) this);
        filter2 = (Button)findViewById(R.id.my_button2);
        filter2.setOnClickListener((OnClickListener)this);
        filter3 = (Button)findViewById(R.id.my_button3);
        filter3.setOnClickListener((OnClickListener)this);
        str = new String("hw2_desktop");
        str2 = new String("hw2_otherdev");
        str3 = new String("hw2_read");
        hw_handler = new Handler();
        hw_log = new File(File.separator + "storage" + File.separator + "sdcard0" + File.separator + "hw2_log.txt");
        try {
			hw_fw = new BufferedWriter(new FileWriter(hw_log));
		} catch (IOException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
		}
        Thread t = new Thread(new Runnable(){
            @Override
            public void run() {		//server receiving thread. server is always waiting for the packet.
            	 try {
 	     			hw2_server = new ServerSocket(7777);	// on port 7777
 	     			hw2_tv.append("server_socket ok!\n");
 	     			hw2_server.setSoTimeout(100000);	
 	     			hw2_tv.append("server_socket time ok!\n");
            	 } catch (IOException e) {
   	     			hw2_tv.append("server_socket error\n");
   	     			e.printStackTrace();
   	     		}
 	     		while(true)
 	     		{
            	  try {
					server = hw2_server.accept();
				
    			 DataInputStream in = new DataInputStream(server.getInputStream());
    			 server_read = new String(in.readUTF() + "\n");		//"desktop from 'ip' " or "phone from 'ip' " is come.
     			 
            	  } catch (IOException e1) {
  					e1.printStackTrace();
  				}
       			
            	 
                hw_handler.post(new Runnable(){		//if i want to change UI, i have to run only one thread. UI can't be updated at mutiple thread.
                    @Override						//handler is the solution for that problem
                    public void run() {
                        
                    	hw2_tv.append(server_read);	
                    	try {
							hw_fw.write(server_read);
						} catch (IOException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
                    }
                });
                try {
					server.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
            	}
            }});
        t.start();
    
       
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
