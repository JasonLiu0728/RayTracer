using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace RayTracerUI
{
    public partial class Form1 : Form
    {
        string sceneSignature = "false";

        string enableShadow = "false";
        string enableGlossyReflection = "false";
        string enableRefraction = "false";
        string width = "0";
        string height = "0";

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Process raytracer = new Process();
            string args;

            args = String.Format("{0} {1} {2} {3} {4} {5}", width, height, sceneSignature, enableShadow, enableGlossyReflection, enableRefraction);

            raytracer.StartInfo.FileName = "C:\\Users\\Jason\\Documents\\Visual Studio 2013\\Projects\\RayTracer\\x64\\Debug\\RayTracer.exe";
            raytracer.StartInfo.Arguments = args;
            raytracer.Start();
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked)
                enableShadow = "true";
            else
                enableShadow = "false";
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox2.Checked)
                enableGlossyReflection = "true";
            else
                enableGlossyReflection = "false";
        }

        private void checkBox3_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox3.Checked)
                enableRefraction = "true";
            else
                enableRefraction = "false";
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            width = textBox1.Text;
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            height = textBox2.Text;
        }

        private void checkBox8_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox8.Checked)
                sceneSignature = "true";
            else
                sceneSignature = "false";
        }

        private void checkBox5_CheckedChanged(object sender, EventArgs e)
        {

        }

        
    }
}
