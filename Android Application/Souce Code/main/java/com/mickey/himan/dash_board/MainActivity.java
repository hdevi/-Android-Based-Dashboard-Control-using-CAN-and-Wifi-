package com.mickey.himan.dash_board;

/**
 * This file Creates a client application on android that communicates
 * with server using TCP sockets.
 * Author: Sanika Balkawade
 * created : 12- march - 2018
 */

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Toast;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;

public class MainActivity extends AppCompatActivity {

    /**
     * Reference declaration for each widget used
     */
    private EditText mServerIP_EditText;
    private EditText mPortNumber_EditText;
    private Button mConnectButton;
    private ImageButton mButton_RadioSetup;
    private ImageButton mButton_MusicPlayer;
    private ImageButton mButton_DisplayPhotos;
    private ImageButton mButton_PlayMovie;

    private String TAG = "Client";
    String IPAddress_str = null, PortNumber_str = null;

    int Port;   //port declaration to take input as only integer
    int mOption = 0;    //variable to select which image to send

    Socket socket = new Socket();   //Socket object created to perform communication


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.d(TAG, "OnCreate Called");

        //Connect all widgets to their respective ID's
        mServerIP_EditText = (EditText) findViewById(R.id.Server);
        mPortNumber_EditText = (EditText) findViewById(R.id.PortNumber);
        mConnectButton = (Button) findViewById(R.id.Button_Connect);
        mButton_RadioSetup = (ImageButton) findViewById(R.id.button1);
        mButton_MusicPlayer = (ImageButton) findViewById(R.id.button2);
        mButton_DisplayPhotos = (ImageButton) findViewById(R.id.button3);
        mButton_PlayMovie = (ImageButton) findViewById(R.id.button4);


        //OnClickListener method called on connect button
        mConnectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                IPAddress_str = mServerIP_EditText.getText().toString().trim();
                PortNumber_str = mPortNumber_EditText.getText().toString().trim();
                //Only take input as Integer for Port number
                Port = Integer.parseInt(PortNumber_str);

                //Check if user entered IP address and port number
                if ((IPAddress_str.length() > 0) && (PortNumber_str.length() > 0))
                {
                    //function called which creates a background thread for communication with server
                    //mOption = 0 is being sent
                    sendMessage(mOption);

                    Log.d(TAG, "IP ADDRESS = " + IPAddress_str + "Port Number" + PortNumber_str);

                }
                //Else part checks if There is valid IP address and Port number entered, If not it gives
                //Toast message
                else {

                    if (IPAddress_str.length() == 0 && PortNumber_str.length() > 0) {
                        Toast.makeText(MainActivity.this, "Please Enter IP address.", Toast.LENGTH_SHORT).show();
                    }
                    if (IPAddress_str.length() > 0 && PortNumber_str.length() == 0) {
                        Toast.makeText(MainActivity.this, "Please Port Number", Toast.LENGTH_SHORT).show();
                    } else {
                        Toast.makeText(MainActivity.this, "Please Enter IP address and Port Number", Toast.LENGTH_SHORT).show();
                    }
                }

            }


        });

        //OnClickListener method called on Radio Image button
        mButton_RadioSetup.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                mOption = 1;
                //moption = 1 is sent in this image is pressed
                sendMessage(mOption);
            }
        });
        //OnClickListener method called on Music Image button
        mButton_MusicPlayer.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mOption = 2;
                //moption = 2 is sent in this image is pressed
                sendMessage(mOption);
            }
        });
        //OnClickListener method called on Gallery Image button
        mButton_DisplayPhotos.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mOption = 3;
                //moption = 3 is sent in this image is pressed
                sendMessage(mOption);
            }
        });
        //OnClickListener method called on Movie Image button
        mButton_PlayMovie.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mOption = 4;
                //moption = 4 is sent in this image is pressed
                sendMessage(mOption);
            }
        });

    }//Oncreate

    /**
     * This function creates a Background Thread and Creates a socket if not created with specific IP address
     * and Portnumber given by User
     * @param msg It is variable which is given on Output stream for specified Image button pressed
     */
    private void sendMessage(final int msg) {
        Log.d(TAG,"In sendMessage with int = "+msg);

    Thread thread = new Thread(new Runnable() {
        @Override
        public void run() {
            try {
                //Checks if socket is connected, If not then it connects to a IP address and Port number
                if(!socket.isConnected()) {

                    socket = new Socket(IPAddress_str,Port);
               }

                Log.d(TAG, "Socket created");

                //stream is created to perform communication between client and server
                OutputStream Out = socket.getOutputStream();

                //Creates a new PrintWriter, without automatic line flushing, from an existing OutputStream
                PrintWriter output = new PrintWriter(Out);

                output.print(msg);
                //flushes the msg on output stream
                output.flush();

                Log.d(TAG, "Message sent");

            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    });

    //Thread is started
    thread.start();
}

    @Override
    protected void onDestroy() {
        super.onDestroy();
        try {
            Log.d(TAG, "Socket is Closing...");
            //Socket is closed when app is being destroyed
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

