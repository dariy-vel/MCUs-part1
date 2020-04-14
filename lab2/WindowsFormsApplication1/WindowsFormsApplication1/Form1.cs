using System;
using System.Data;
using System.Linq;
using System.Windows.Forms;
using System.IO.Ports;
using System.Drawing;
using System.Threading;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        const int delay = 550;
        public Form1()
        {
            InitializeComponent();
        }
        
        private void comboBox1_Click(object sender, EventArgs e)
        {
            int num;
            comboBox1.Items.Clear();
            string[] ports = SerialPort.GetPortNames().OrderBy(a => a.Length > 3 && int.TryParse(a.Substring(3), out num) ? num : 0).ToArray();
            comboBox1.Items.AddRange(ports);
        }

        private void buttonOpenPort_Click(object sender, EventArgs e)
        {
            if (!serialPort1.IsOpen)
                try
                {
                    serialPort1.PortName = comboBox1.Text;
                    serialPort1.Open();
                    buttonOpenPort.Text = "Close";
                    comboBox1.Enabled = false;
                    button1.Visible = true;
                    button2.Visible = true;
                }
                catch
                {
                    MessageBox.Show("Port " + comboBox1.Text + " is invalid!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            else
            {
                serialPort1.Close();
                buttonOpenPort.Text = "Open";
                comboBox1.Enabled = true;
                button1.Visible = false;
                button2.Visible = false;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            byte[] b1 = new byte[1];
            b1[0] = 0xA1;
            serialPort1.Write(b1, 0, 1);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            byte[] b1 = new byte[1];
            b1[0] = 0xB1;
            serialPort1.Write(b1, 0, 1);
        }

        private void clearAllLed()
        {
            panel1.BackColor = Color.SkyBlue;
            panel2.BackColor = Color.SkyBlue;
            panel3.BackColor = Color.SkyBlue;
            panel4.BackColor = Color.SkyBlue;
            panel5.BackColor = Color.SkyBlue;
            panel6.BackColor = Color.SkyBlue;
            panel7.BackColor = Color.SkyBlue;
            panel8.BackColor = Color.SkyBlue;
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            byte commandFromArduino = (byte)serialPort1.ReadByte();
            Console.WriteLine(commandFromArduino);
            if (commandFromArduino == 0xA1)
            {
                panel4.BackColor = Color.Yellow;
                panel5.BackColor = Color.Yellow;
                Thread.Sleep(delay);
                clearAllLed();
                panel3.BackColor = Color.Yellow;
                panel6.BackColor = Color.Yellow;
                Thread.Sleep(delay);
                clearAllLed();
                panel2.BackColor = Color.Yellow;
                panel7.BackColor = Color.Yellow;
                Thread.Sleep(delay);
                clearAllLed();
                panel1.BackColor = Color.Yellow;
                panel8.BackColor = Color.Yellow;
                Thread.Sleep(delay);
                clearAllLed();
            }
            else if (commandFromArduino == 0xB1)
            {
                panel8.BackColor = Color.Yellow;
                Thread.Sleep(delay);
                panel8.BackColor = Color.SkyBlue;
                panel6.BackColor = Color.Yellow;
                Thread.Sleep(delay);
                panel6.BackColor = Color.SkyBlue;
                panel4.BackColor = Color.Yellow;
                Thread.Sleep(delay);
                panel4.BackColor = Color.SkyBlue;
                panel2.BackColor = Color.Yellow;
                Thread.Sleep(delay);
                panel2.BackColor = Color.SkyBlue;
                panel7.BackColor = Color.Yellow;
                Thread.Sleep(delay);
                panel7.BackColor = Color.SkyBlue;
                panel5.BackColor = Color.Yellow;
                Thread.Sleep(delay);
                panel5.BackColor = Color.SkyBlue;
                panel3.BackColor = Color.Yellow;
                Thread.Sleep(delay);
                panel3.BackColor = Color.SkyBlue;
                panel1.BackColor = Color.Yellow;
                Thread.Sleep(delay);
                panel1.BackColor = Color.SkyBlue;
            }
        }
    }
}
