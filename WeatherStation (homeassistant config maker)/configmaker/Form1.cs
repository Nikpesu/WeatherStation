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
            updateRTBa();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
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
                imena = imena.Substring(0, imena.Length - 2) + ")";

                try { tmp += "#" + new string('-', (66 - imena.Length) / 2) + imena + new string('-', 66 - ((66 - imena.Length) / 2) - imena.Length) + "\n\n"; }
                catch(Exception ex) { tmp += "#" + imena + "\n\n"; }
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
                                    tmp += "  - state_topic: \"" + tb1 + "/"+ senzor + "/"+ par + "\"\r\n    unique_id: \"" + tb1 + "_"+senzor+"_"+ par + "\"\r\n    name: \"" + tb1 + "_"+ senzor + "_"+ par +"\"\r\n    unit_of_measurement: \""+ mjerneJedinice[i++] + "\"\n";

                                    }
                                 tmp += "\n";
                            }
                        break;

                        case "BMP280":
                            {
                                string[] parametri = { "temp", "press", "alt", "status"};
                                string[] mjerneJedinice = { "°C", "hPa", "m", ""};
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
                                string[] mjerneJedinice = { "risk", "ppb", "ppm", ""};
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
                                tmp += "  # " + senzor + " "+ tb1+ "\n";
                                foreach (string par in parametri)
                                {
                                    tmp += "  - state_topic: \"" + tb1 + "/" + senzor + "/" + par + "\"\r\n    unique_id: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    name: \"" + tb1 + "_" + senzor + "_" + par + "\"\r\n    unit_of_measurement: \"" + mjerneJedinice[i++] + "\"\n";

                                }
                                tmp += "\n";
                            }
                        break;

                    }
                }
                tmp += "#"+new String('-', 66);
                richTextBox1.Text = tmp;
            }
            else richTextBox1.Text = "";
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
    }
}
