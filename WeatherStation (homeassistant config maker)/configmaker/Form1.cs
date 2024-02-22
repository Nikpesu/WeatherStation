using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.VisualStyles.VisualStyleElement;
using static System.Windows.Forms.VisualStyles.VisualStyleElement.Tab;

namespace configmaker
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            checkedListBox1.Items.Add("SGP30");
            checkedListBox1.Items.Add("SHT31");
            checkedListBox1.Items.Add("BMP280");
            checkedListBox1.Items.Add("AHT2x");
            checkedListBox1.Items.Add("ENS160");
            checkedListBox1.Items.Add("SCD4x");
            checkedListBox1.Items.Add("PMSx003");
            updateRTBa();
            makeHTML();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
        }


        void makeHTML()
        {
            string[][] parametri = new string[][] {
                //data, label, category, placeholder, value, type
                new string[] { "wifi_ssid", "WiFi SSID:", "0", "SSID", "", "text"},
                new string[] { "wifi_pass", "WiFi Password:", "0", "PASS", "", "password"},
                new string[] { "mqtt_server", "MQTT Server:", "0", "Mosquitto IP", "", "text"},
                new string[] { "mqtt_port", "MQTT Port:", "0", "1883", "", "number"},
                new string[] { "mqtt_user", "MQTT Username:", "0", "Mosquitto Username", "", "text"},
                new string[] { "mqtt_password", "MQTT Password", "0", "Mosquitto Passowrd", "", "password"},
                new string[] { "mqtt_messageRoot", "MQTT Message Root (\"msgroot\" + \"/sensor/data\"):", "0", "ws123", "", "text"},
                new string[] { "BMP280_toggle", "BMP280", "1", "BMP280", "", "checkbox"},
                new string[] { "SHT31_toggle","SHT31","1", "", "", "checkbox"},
                new string[] { "SGP30_toggle","SGP30","1", "", "", "checkbox"},
                new string[] { "AHT2x_toggle","AHT2x","1", "", "", "checkbox"},
                new string[] { "ENS160_toggle","ENS160","1", "", "", "checkbox"},
                new string[] { "SCD4x_toggle", "SCD4x", "1", "", "", "checkbox"},
                new string[] { "PMSx003_toggle", "PMSx003", "1", "", "", "checkbox"},
                new string[] { "mdns_hostname", "mDNS hostname (without .local):", "2", "mdns", "", "text"},
                new string[] { "hotspot_ssid", "Hotspot SSID:", "2", "ssid", "", "text"},
                new string[] { "hotspot_pass", "Hotspot Password:", "2", "pass", "", "password"},
                new string[] { "lowPowerMode_toggle", "Low Power Mode (toggle / refresh|ON Time)", "3", "", "", "checkbox"},
                new string[] { "refreshTime","", "3", "30", "", "number" }
            };
            // <html> <head> <style></style> <script defer>
            string output = "<!DOCTYPE html>\r\n<html>\r\n\r\n<head>\r\n  <title>Weatherstation</title>\r\n  <meta name=\"viewport\" content=\"initial-scale=1.0\">\r\n  <style>\r\n    form {\r\n      width: 75%;\r\n      font-family: Arial, Helvetica, sans-serif;\r\n      font-size: large;\r\n      font-weight: bold;\r\n    }\r\n\r\n    #myForm>div {\r\n\r\n      display: flex;\r\n      justify-content: center;\r\n      align-content: center;\r\n      flex-direction: column;\r\n      width: auto;\r\n      align-content: center;\r\n    }\r\n\r\n    body {\r\n      background-color: rgba(196, 196, 196, 0.411);\r\n      font-family: Arial, Helvetica, sans-serif;\r\n      display: flex;\r\n      justify-content: center;\r\n      align-content: center;\r\n      flex-direction: column;\r\n      width: auto;\r\n      align-content: center;\r\n      flex-wrap: wrap;\r\n    }\r\n\r\n    .senzori {\r\n      display: flex;\r\n      justify-content: space-around;\r\n      align-items: center;\r\n      width: 100%;\r\n      flex-direction: row;\r\n\r\n    }\r\n\r\n    .senzor {\r\n      display: flex;\r\n      flex-direction: column;\r\n    }\r\n\r\n    label {\r\n      align-self: center;\r\n      margin-top: 1em;\r\n    }\r\n\r\n    input {\r\n      height: 1.7em;\r\n      font-size: 1.6em;\r\n      background-color: gainsboro;\r\n      border-radius: 0.3em;\r\n      border: gray 1px solid;\r\n      transition: 0.2s;\r\n      color: rgb(0, 31, 78);\r\n    }\r\n\r\n    input[type=\"button\"] {\r\n      height: 3em;\r\n      margin-top: 1em;\r\n\r\n    }\r\n    input:hover {\r\n      border-color: rgb(82, 82, 82);\r\n      background-color: rgb(175, 175, 175);\r\n      transition: 0.2s;\r\n    }\r\n    \r\n    input[type=\"button\"]:active {\r\n      border-color: rgb(80, 78, 78);\r\n      background-color: rgb(100, 112, 117);\r\n      color: white;\r\n\r\n    }\r\n\r\n    input[type=number]::-webkit-inner-spin-button,\r\n    input[type=number]::-webkit-outer-spin-button {\r\n      -webkit-appearance: none;\r\n      margin: 1;\r\n    }\r\n\r\n    input[type=number] {\r\n      -moz-appearance: textfield;\r\n      appearance: textfield;\r\n    }\r\n  </style>\r\n  <script defer>";

            output += "\r\n    function sendData() {\r\n      // Get form values\n      ";

            // Get form values
            foreach (string[] a in parametri)
                output += "const " + a[0] + " = document.getElementById(\"" + a[0] + "\").value;\r\n      ";

            // Create data object
            output += "\r\n      const data = {";
            foreach (string[] a in parametri)
                output += a[0] + ",";
            output = output.TrimEnd(',');
            output += "};\r\n";

            output += "// Send data using fetch API\r\n      fetch('/data', {\r\n        method: 'POST',\r\n        headers: { 'Content-Type': 'application/json' },\r\n        body: JSON.stringify(data)\r\n      })\r\n        .then(response => {\r\n          console.log('Data sent:', data);\r\n          // Reset form after data is sent\r\n          //document.getElementById(\"myForm\").reset();\r\n        })\r\n        .catch(error => console.error('Error:', error));";


            //</script> </head><body> <h1></h1> <form>
            output += "}\r\n  </script>\r\n</head>\r\n\r\n<body><h1 style=\"align-self: center;\">Weatherstation</h1>\r\n  <form id=\"myForm\" onsubmit=\"return false;\">\r\n    <div>";

            bool prvi1 = true;
            bool prvi2 = true;
            bool prvi3 = true;
            foreach (string[] a in parametri)
            {
                if (a[2] == "0")
                {
                    output += "      <label for=\"" + a[0] + "\">" + a[1] + "</label>\r\n      <input type=\"" + a[5] + "\" placeholder=\"" + a[3] + "\" required name=\"" + a[0] + "\"  id=\"" + a[0] + "\" value=\"" + a[4] + "\"><br>\r\n";

                }
                else if (a[2] == "1")
                {
                    if (prvi1) { prvi1 = false; output += "\r\n      <label for=\"sens\">Senzori:</label>\r\n      <div id=\"sens\" class=\"senzori\">"; }
                    output += "\r\n        <div class=\"senzor\">\r\n          " + "      <label for=\"" + a[0] + "\">" + a[1] + "</label>\r\n      <input type=\"" + a[5] + "\" placeholder=\"" + a[3] + "\" required name=\"" + a[0] + "\"  id=\"" + a[0] + "\" value=\"" + a[4] + "\"><br>\r\n" + "</div>";
                }
                else if (a[2] == "2")
                {
                    if (prvi2) { prvi2 = false; output += "\r\n\r\n      </div><br>\r\n"; }
                    output += "      <label for=\"" + a[0] + "\">" + a[1] + "</label>\r\n      <input type=\"" + a[5] + "\" placeholder=\"" + a[3] + "\" required name=\"" + a[0] + "\"  id=\"" + a[0] + "\" value=\"" + a[4] + "\"><br>\r\n";
                }
                else if (a[2] == "3")
                {
                    if (prvi3) { prvi3 = false; output += "\r\n      <div id=\"s\">\r\n        <div class=\"senzor\">"; }
                    if (a[1] != "") output += "      <label for=\"" + a[0] + "\">" + a[1] + "</label>";
                    output += "<input type =\"" + a[5] + "\" placeholder=\"" + a[3] + "\" required name=\"" + a[0] + "\"  id=\"" + a[0] + "\" value=\"" + a[4] + "\"><br>\r\n";
                }
            }

            output += "\r\n        </div>\r\n      </div><br>\r\n\r\n      <input id=\"subm\" type=\"button\" value=\"Submit\" onclick=\"sendData()\" >\r\n\r\n     </div>\r\n       <br><p></p><br><a href=\"currentConfig\"><button type=\"button\"> Current config </button></a>\r\n </form>\r\n</body>\r\n\r\n</html>";
            richTextBox2.Text = output;



        }

        private void checkedListBox1_KeyUp(object sender, KeyEventArgs e)
        {
            updateRTBa();

        }

        private void checkedListBox1_MouseUp(object sender, MouseEventArgs e)
        {

            updateRTBa();
        }

        private void textBox1_KeyUp(object sender, KeyEventArgs e)
        {

            updateRTBa();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try { Clipboard.SetText(richTextBox1.Text); } catch { }
        }
        private void updateRTBa()
        {
            if (checkedListBox1.CheckedItems.Count > 0)
            {
                string tb1 = textBox1.Text;
                if (tb1 == "") tb1 = "placeholderName";
                string tmp = "";
                string imena = tb1 + " (";
                foreach (Object da in checkedListBox1.CheckedItems)
                {
                    imena += da.ToString() + ",";
                }
                imena = imena.TrimEnd(',') + ")";

                try { tmp += "#" + new string('-', (66 - imena.Length) / 2) + imena + new string('-', 66 - ((66 - imena.Length) / 2) - imena.Length) + "\n\n"; }
                catch (Exception ex) { tmp += "#" + imena + "\n\n"; }
                foreach (Object da in checkedListBox1.CheckedItems)
                {
                    switch (da.ToString())
                    {
                        case "SGP30":
                            {
                                string[] parametri = { "TVOC", "eCO2", "ETH", "H2" };
                                string[] mjerneJedinice = { "ppb", "ppm", "", "" };
                                string senzor = "SGP30";
                                int i = 0;
                                tmp += "  # " + senzor + " " + tb1 + "\n";
                                foreach (string par in parametri)
                                {
                                    tmp += "  - state_topic: \"" + tb1 + "/" + senzor + "/" + par + "\"\r\n    unique_id: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    name: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    unit_of_measurement: \"" + mjerneJedinice[i++] + "\"\n";

                                }
                                tmp += "\n";
                            }
                            break;

                        case "BMP280":
                            {
                                string[] parametri = { "temp", "press", "alt", "status" };
                                string[] mjerneJedinice = { "°C", "hPa", "m", "" };
                                string senzor = "BMP280";
                                int i = 0;
                                tmp += "  # " + senzor + " " + tb1 + "\n";
                                foreach (string par in parametri)
                                {
                                    tmp += "  - state_topic: \"" + tb1 + "/" + senzor + "/" + par + "\"\r\n    unique_id: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    name: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    unit_of_measurement: \"" + mjerneJedinice[i++] + "\"\n";

                                }
                                tmp += "\n";
                            }
                            break;

                        case "SHT31":
                            {
                                string[] parametri = { "temp", "hum", "status" };
                                string[] mjerneJedinice = { "°C", "%", "" };
                                string senzor = "SHT31";
                                int i = 0;
                                tmp += "  # " + senzor + " " + tb1 + "\n";
                                foreach (string par in parametri)
                                {
                                    tmp += "  - state_topic: \"" + tb1 + "/" + senzor + "/" + par + "\"\r\n    unique_id: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    name: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    unit_of_measurement: \"" + mjerneJedinice[i++] + "\"\n";

                                }
                                tmp += "\n";
                            }
                            break;

                        case "ENS160":
                            {
                                string[] parametri = { "AQI", "TVOC", "ECO2", "status" };
                                string[] mjerneJedinice = { "risk", "ppb", "ppm", "" };
                                string senzor = "ENS160";
                                int i = 0;
                                tmp += "  # " + senzor + " " + tb1 + "\n";
                                foreach (string par in parametri)
                                {
                                    tmp += "  - state_topic: \"" + tb1 + "/" + senzor + "/" + par + "\"\r\n    unique_id: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    name: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    unit_of_measurement: \"" + mjerneJedinice[i++] + "\"\n";

                                }
                                tmp += "\n";
                            }
                            break;

                        case "AHT2x":
                            {
                                string[] parametri = { "temp", "hum", "status" };
                                string[] mjerneJedinice = { "°C", "%", "" };
                                string senzor = "AHT2x";
                                int i = 0;
                                tmp += "  # " + senzor + " " + tb1 + "\n";
                                foreach (string par in parametri)
                                {
                                    tmp += "  - state_topic: \"" + tb1 + "/" + senzor + "/" + par + "\"\r\n    unique_id: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    name: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    unit_of_measurement: \"" + mjerneJedinice[i++] + "\"\n";

                                }
                                tmp += "\n";
                            }
                            break;

                        case "SCD4x":
                            {
                                string[] parametri = { "temp", "hum", "CO2", "status" };
                                string[] mjerneJedinice = { "°C", "%", "ppm", "" };
                                string senzor = "SCD4x";
                                int i = 0;
                                tmp += "  # " + senzor + " " + tb1 + "\n";
                                foreach (string par in parametri)
                                {
                                    tmp += "  - state_topic: \"" + tb1 + "/" + senzor + "/" + par + "\"\r\n    unique_id: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    name: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    unit_of_measurement: \"" + mjerneJedinice[i++] + "\"\n";

                                }
                                tmp += "\n";
                            }
                            break;

                        case "PMSx003":
                            {
                                string[] parametri = {      "pm1.0", "pm2.5", "pm10",  "n0p3",     "n0p5",     "n1p0",     "n2p5",     "n5p0",     "n10p0",    "hcho",  "temp", "hum" };
                                string[] mjerneJedinice = { "μg/m3", "μg/m3", "μg/m3", "#/100cm3", "#/100cm3", "#/100cm3", "#/100cm3", "#/100cm3", "#/100cm3", "mg/m3", "°C",   "%" };
                                string senzor = "PMSxxxx";
                                int i = 0;
                                tmp += "  # " + senzor + " " + tb1 + "\n";
                                foreach (string par in parametri)
                                {
                                    tmp += "  - state_topic: \"" + tb1 + "/" + senzor + "/" + par + "\"\r\n    unique_id: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    name: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    unit_of_measurement: \"" + mjerneJedinice[i++] + "\"\n";

                                }
                                tmp += "\n";
                            }
                            break;

                    }
                }
                tmp += "#" + new String('-', 66);
                richTextBox1.Text = tmp;
            }
            else richTextBox1.Text = "";
        }

        private void richTextBox2_TextChanged(object sender, EventArgs e)
        {

        }
    }
}