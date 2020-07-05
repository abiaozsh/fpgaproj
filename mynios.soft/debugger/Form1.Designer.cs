using System.Windows.Forms;
namespace WindowsFormsApplication1
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			this.button1 = new System.Windows.Forms.Button();
			this.button4 = new System.Windows.Forms.Button();
			this.textBox4 = new System.Windows.Forms.TextBox();
			this.comboBox1 = new System.Windows.Forms.ComboBox();
			this.textBox1 = new System.Windows.Forms.TextBox();
			this.textBox2 = new System.Windows.Forms.TextBox();
			this.button2 = new System.Windows.Forms.Button();
			this.checkBox1 = new System.Windows.Forms.CheckBox();
			this.button3 = new System.Windows.Forms.Button();
			this.button5 = new System.Windows.Forms.Button();
			this.button6 = new System.Windows.Forms.Button();
			this.button7 = new System.Windows.Forms.Button();
			this.button8 = new System.Windows.Forms.Button();
			this.comboBox2 = new System.Windows.Forms.ComboBox();
			this.button9 = new System.Windows.Forms.Button();
			this.button10 = new System.Windows.Forms.Button();
			this.button11 = new System.Windows.Forms.Button();
			this.textBox3 = new System.Windows.Forms.TextBox();
			this.button12 = new System.Windows.Forms.Button();
			this.button13 = new System.Windows.Forms.Button();
			this.trackBar1 = new System.Windows.Forms.TrackBar();
			this.checkBox2 = new System.Windows.Forms.CheckBox();
			this.checkBox3 = new System.Windows.Forms.CheckBox();
			this.button14 = new System.Windows.Forms.Button();
			this.button15 = new System.Windows.Forms.Button();
			this.checkBox4 = new System.Windows.Forms.CheckBox();
			this.checkBox5 = new System.Windows.Forms.CheckBox();
			this.checkBox6 = new System.Windows.Forms.CheckBox();
			this.checkBox7 = new System.Windows.Forms.CheckBox();
			((System.ComponentModel.ISupportInitialize)(this.trackBar1)).BeginInit();
			this.SuspendLayout();
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(105, 13);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(61, 23);
			this.button1.TabIndex = 0;
			this.button1.Text = "connect";
			this.button1.UseVisualStyleBackColor = true;
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// button4
			// 
			this.button4.Location = new System.Drawing.Point(172, 13);
			this.button4.Name = "button4";
			this.button4.Size = new System.Drawing.Size(82, 23);
			this.button4.TabIndex = 7;
			this.button4.Text = "getstatus";
			this.button4.UseVisualStyleBackColor = true;
			this.button4.Click += new System.EventHandler(this.button4_Click);
			// 
			// textBox4
			// 
			this.textBox4.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.textBox4.Location = new System.Drawing.Point(12, 71);
			this.textBox4.Multiline = true;
			this.textBox4.Name = "textBox4";
			this.textBox4.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
			this.textBox4.Size = new System.Drawing.Size(1083, 700);
			this.textBox4.TabIndex = 8;
			this.textBox4.TextChanged += new System.EventHandler(this.textBox4_TextChanged);
			// 
			// comboBox1
			// 
			this.comboBox1.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBox1.FormattingEnabled = true;
			this.comboBox1.Location = new System.Drawing.Point(12, 13);
			this.comboBox1.Name = "comboBox1";
			this.comboBox1.Size = new System.Drawing.Size(76, 20);
			this.comboBox1.TabIndex = 17;
			this.comboBox1.SelectedIndexChanged += new System.EventHandler(this.comboBox1_SelectedIndexChanged);
			// 
			// textBox1
			// 
			this.textBox1.Location = new System.Drawing.Point(428, 13);
			this.textBox1.Name = "textBox1";
			this.textBox1.Size = new System.Drawing.Size(100, 21);
			this.textBox1.TabIndex = 18;
			this.textBox1.Text = "0";
			// 
			// textBox2
			// 
			this.textBox2.Location = new System.Drawing.Point(666, 13);
			this.textBox2.Name = "textBox2";
			this.textBox2.Size = new System.Drawing.Size(95, 21);
			this.textBox2.TabIndex = 19;
			// 
			// button2
			// 
			this.button2.Location = new System.Drawing.Point(531, 11);
			this.button2.Name = "button2";
			this.button2.Size = new System.Drawing.Size(50, 23);
			this.button2.TabIndex = 20;
			this.button2.Text = "getmem";
			this.button2.UseVisualStyleBackColor = true;
			this.button2.Click += new System.EventHandler(this.button2_Click_1);
			// 
			// checkBox1
			// 
			this.checkBox1.AutoSize = true;
			this.checkBox1.Location = new System.Drawing.Point(368, 15);
			this.checkBox1.Name = "checkBox1";
			this.checkBox1.Size = new System.Drawing.Size(48, 16);
			this.checkBox1.TabIndex = 21;
			this.checkBox1.Text = "halt";
			this.checkBox1.UseVisualStyleBackColor = true;
			this.checkBox1.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
			// 
			// button3
			// 
			this.button3.Location = new System.Drawing.Point(260, 13);
			this.button3.Name = "button3";
			this.button3.Size = new System.Drawing.Size(82, 23);
			this.button3.TabIndex = 22;
			this.button3.Text = "step";
			this.button3.UseVisualStyleBackColor = true;
			this.button3.Click += new System.EventHandler(this.button3_Click_1);
			// 
			// button5
			// 
			this.button5.Location = new System.Drawing.Point(767, 12);
			this.button5.Name = "button5";
			this.button5.Size = new System.Drawing.Size(87, 23);
			this.button5.TabIndex = 23;
			this.button5.Text = "clear buffer";
			this.button5.UseVisualStyleBackColor = true;
			this.button5.Click += new System.EventHandler(this.button5_Click);
			// 
			// button6
			// 
			this.button6.Location = new System.Drawing.Point(587, 11);
			this.button6.Name = "button6";
			this.button6.Size = new System.Drawing.Size(50, 23);
			this.button6.TabIndex = 24;
			this.button6.Text = "setmem";
			this.button6.UseVisualStyleBackColor = true;
			this.button6.Click += new System.EventHandler(this.button6_Click);
			// 
			// button7
			// 
			this.button7.Location = new System.Drawing.Point(531, 37);
			this.button7.Name = "button7";
			this.button7.Size = new System.Drawing.Size(49, 23);
			this.button7.TabIndex = 25;
			this.button7.Text = "test";
			this.button7.UseVisualStyleBackColor = true;
			this.button7.Click += new System.EventHandler(this.button7_Click);
			// 
			// button8
			// 
			this.button8.Location = new System.Drawing.Point(12, 42);
			this.button8.Name = "button8";
			this.button8.Size = new System.Drawing.Size(86, 23);
			this.button8.TabIndex = 26;
			this.button8.Text = "upload bios";
			this.button8.UseVisualStyleBackColor = true;
			this.button8.Click += new System.EventHandler(this.button8_Click);
			// 
			// comboBox2
			// 
			this.comboBox2.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.comboBox2.FormattingEnabled = true;
			this.comboBox2.Items.AddRange(new object[] {
            "0",
            "1",
            "2",
            "4",
            "8",
            "16",
            "32",
            "64"});
			this.comboBox2.Location = new System.Drawing.Point(428, 40);
			this.comboBox2.Name = "comboBox2";
			this.comboBox2.Size = new System.Drawing.Size(76, 20);
			this.comboBox2.TabIndex = 27;
			// 
			// button9
			// 
			this.button9.Location = new System.Drawing.Point(185, 42);
			this.button9.Name = "button9";
			this.button9.Size = new System.Drawing.Size(75, 23);
			this.button9.TabIndex = 28;
			this.button9.Text = "reset";
			this.button9.UseVisualStyleBackColor = true;
			this.button9.Click += new System.EventHandler(this.button9_Click);
			// 
			// button10
			// 
			this.button10.Location = new System.Drawing.Point(729, 40);
			this.button10.Name = "button10";
			this.button10.Size = new System.Drawing.Size(75, 27);
			this.button10.TabIndex = 29;
			this.button10.Text = "MAKE BOOT";
			this.button10.UseVisualStyleBackColor = true;
			this.button10.Click += new System.EventHandler(this.button10_Click);
			// 
			// button11
			// 
			this.button11.Location = new System.Drawing.Point(104, 42);
			this.button11.Name = "button11";
			this.button11.Size = new System.Drawing.Size(75, 23);
			this.button11.TabIndex = 30;
			this.button11.Text = "upload dos";
			this.button11.UseVisualStyleBackColor = true;
			this.button11.Click += new System.EventHandler(this.button11_Click);
			// 
			// textBox3
			// 
			this.textBox3.Location = new System.Drawing.Point(810, 42);
			this.textBox3.Name = "textBox3";
			this.textBox3.Size = new System.Drawing.Size(44, 21);
			this.textBox3.TabIndex = 31;
			this.textBox3.Text = "?";
			// 
			// button12
			// 
			this.button12.Location = new System.Drawing.Point(674, 42);
			this.button12.Name = "button12";
			this.button12.Size = new System.Drawing.Size(49, 23);
			this.button12.TabIndex = 32;
			this.button12.Text = "f0";
			this.button12.UseVisualStyleBackColor = true;
			this.button12.Click += new System.EventHandler(this.button12_Click);
			// 
			// button13
			// 
			this.button13.Location = new System.Drawing.Point(1013, 35);
			this.button13.Name = "button13";
			this.button13.Size = new System.Drawing.Size(79, 23);
			this.button13.TabIndex = 33;
			this.button13.Text = "makeimg";
			this.button13.UseVisualStyleBackColor = true;
			this.button13.Click += new System.EventHandler(this.button13_Click);
			// 
			// trackBar1
			// 
			this.trackBar1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
			this.trackBar1.Location = new System.Drawing.Point(1101, 15);
			this.trackBar1.Maximum = 8192;
			this.trackBar1.Name = "trackBar1";
			this.trackBar1.Orientation = System.Windows.Forms.Orientation.Vertical;
			this.trackBar1.Size = new System.Drawing.Size(45, 758);
			this.trackBar1.TabIndex = 34;
			this.trackBar1.Scroll += new System.EventHandler(this.trackBar1_Scroll);
			// 
			// checkBox2
			// 
			this.checkBox2.AutoSize = true;
			this.checkBox2.Location = new System.Drawing.Point(874, 15);
			this.checkBox2.Name = "checkBox2";
			this.checkBox2.Size = new System.Drawing.Size(78, 16);
			this.checkBox2.TabIndex = 35;
			this.checkBox2.Text = "vga block";
			this.checkBox2.UseVisualStyleBackColor = true;
			this.checkBox2.CheckedChanged += new System.EventHandler(this.checkBox2_CheckedChanged);
			// 
			// checkBox3
			// 
			this.checkBox3.AutoSize = true;
			this.checkBox3.Location = new System.Drawing.Point(874, 42);
			this.checkBox3.Name = "checkBox3";
			this.checkBox3.Size = new System.Drawing.Size(48, 16);
			this.checkBox3.TabIndex = 36;
			this.checkBox3.Text = "test";
			this.checkBox3.UseVisualStyleBackColor = true;
			this.checkBox3.CheckedChanged += new System.EventHandler(this.checkBox3_CheckedChanged);
			// 
			// button14
			// 
			this.button14.Location = new System.Drawing.Point(1013, 8);
			this.button14.Name = "button14";
			this.button14.Size = new System.Drawing.Size(49, 23);
			this.button14.TabIndex = 38;
			this.button14.Text = "write";
			this.button14.UseVisualStyleBackColor = true;
			this.button14.Click += new System.EventHandler(this.button14_Click);
			// 
			// button15
			// 
			this.button15.Location = new System.Drawing.Point(958, 8);
			this.button15.Name = "button15";
			this.button15.Size = new System.Drawing.Size(49, 23);
			this.button15.TabIndex = 37;
			this.button15.Text = "rnd";
			this.button15.UseVisualStyleBackColor = true;
			this.button15.Click += new System.EventHandler(this.button15_Click);
			// 
			// checkBox4
			// 
			this.checkBox4.AutoSize = true;
			this.checkBox4.Checked = true;
			this.checkBox4.CheckState = System.Windows.Forms.CheckState.Checked;
			this.checkBox4.Location = new System.Drawing.Point(587, 42);
			this.checkBox4.Name = "checkBox4";
			this.checkBox4.Size = new System.Drawing.Size(15, 14);
			this.checkBox4.TabIndex = 39;
			this.checkBox4.UseVisualStyleBackColor = true;
			// 
			// checkBox5
			// 
			this.checkBox5.AutoSize = true;
			this.checkBox5.Checked = true;
			this.checkBox5.CheckState = System.Windows.Forms.CheckState.Checked;
			this.checkBox5.Location = new System.Drawing.Point(608, 42);
			this.checkBox5.Name = "checkBox5";
			this.checkBox5.Size = new System.Drawing.Size(15, 14);
			this.checkBox5.TabIndex = 40;
			this.checkBox5.UseVisualStyleBackColor = true;
			// 
			// checkBox6
			// 
			this.checkBox6.AutoSize = true;
			this.checkBox6.Checked = true;
			this.checkBox6.CheckState = System.Windows.Forms.CheckState.Checked;
			this.checkBox6.Location = new System.Drawing.Point(629, 42);
			this.checkBox6.Name = "checkBox6";
			this.checkBox6.Size = new System.Drawing.Size(15, 14);
			this.checkBox6.TabIndex = 41;
			this.checkBox6.UseVisualStyleBackColor = true;
			// 
			// checkBox7
			// 
			this.checkBox7.AutoSize = true;
			this.checkBox7.Checked = true;
			this.checkBox7.CheckState = System.Windows.Forms.CheckState.Checked;
			this.checkBox7.Location = new System.Drawing.Point(650, 42);
			this.checkBox7.Name = "checkBox7";
			this.checkBox7.Size = new System.Drawing.Size(15, 14);
			this.checkBox7.TabIndex = 42;
			this.checkBox7.UseVisualStyleBackColor = true;
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1156, 783);
			this.Controls.Add(this.checkBox7);
			this.Controls.Add(this.checkBox6);
			this.Controls.Add(this.checkBox5);
			this.Controls.Add(this.checkBox4);
			this.Controls.Add(this.button14);
			this.Controls.Add(this.button15);
			this.Controls.Add(this.checkBox3);
			this.Controls.Add(this.checkBox2);
			this.Controls.Add(this.trackBar1);
			this.Controls.Add(this.button13);
			this.Controls.Add(this.button12);
			this.Controls.Add(this.textBox3);
			this.Controls.Add(this.button11);
			this.Controls.Add(this.button10);
			this.Controls.Add(this.button9);
			this.Controls.Add(this.comboBox2);
			this.Controls.Add(this.button8);
			this.Controls.Add(this.button7);
			this.Controls.Add(this.button6);
			this.Controls.Add(this.button5);
			this.Controls.Add(this.button3);
			this.Controls.Add(this.checkBox1);
			this.Controls.Add(this.button2);
			this.Controls.Add(this.textBox2);
			this.Controls.Add(this.textBox1);
			this.Controls.Add(this.comboBox1);
			this.Controls.Add(this.textBox4);
			this.Controls.Add(this.button4);
			this.Controls.Add(this.button1);
			this.Name = "Form1";
			this.Load += new System.EventHandler(this.Form1_Load);
			((System.ComponentModel.ISupportInitialize)(this.trackBar1)).EndInit();
			this.ResumeLayout(false);
			this.PerformLayout();

        }


        #endregion

		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Button button4;
		private System.Windows.Forms.TextBox textBox4;
		private System.Windows.Forms.ComboBox comboBox1;
        private TextBox textBox1;
        private TextBox textBox2;
        private Button button2;
        private CheckBox checkBox1;
        private Button button3;
        private Button button5;
        private Button button6;
		private Button button7;
		private Button button8;
		private ComboBox comboBox2;
        private Button button9;
        private Button button10;
        private Button button11;
		private TextBox textBox3;
		private Button button12;
		private Button button13;
		private TrackBar trackBar1;
		private CheckBox checkBox2;
		private CheckBox checkBox3;
		private Button button14;
		private Button button15;
		private CheckBox checkBox4;
		private CheckBox checkBox5;
		private CheckBox checkBox6;
		private CheckBox checkBox7;

	}
}

