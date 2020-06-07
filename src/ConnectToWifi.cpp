

// // connect to wifi – returns true if successful or false if not
// boolean ConnectToWifi(ssid, password)
// {
//     boolean state = true;
//     int i = 0;

//     WiFi.begin(ssid, password);
//     WiFi.setSleep(false);
    
//     Serial.println("");
//     Serial.println("Connecting to WiFi");

//     // Wait for connection
//     Serial.print("Connecting");
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         delay(500);
//         Serial.print(".");
//         if (i > 20)
//         {
//             state = false;
//             break;
//         }
//         i++;
//     }
//     if (state)
//     {
//         Serial.println("");
//         Serial.print("Connected to ");
//         Serial.println(ssid);
//         Serial.print("IP address: ");
//         Serial.println(WiFi.localIP());
//     }
//     else
//     {
//         Serial.println("");
//         Serial.println("Connection failed.");
//     }

//     return state;
// }
