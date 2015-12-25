namespace JWebTop_CSharp_Demo
{
    partial class MainForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.btnNewNote = new System.Windows.Forms.ToolStripButton();
            this.btnDelNote = new System.Windows.Forms.ToolStripButton();
            this.splitMain = new System.Windows.Forms.SplitContainer();
            this.toolStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitMain)).BeginInit();
            this.splitMain.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnNewNote,
            this.btnDelNote});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(424, 25);
            this.toolStrip1.TabIndex = 4;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // btnNewNote
            // 
            this.btnNewNote.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btnNewNote.Image = ((System.Drawing.Image)(resources.GetObject("btnNewNote.Image")));
            this.btnNewNote.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnNewNote.Name = "btnNewNote";
            this.btnNewNote.Size = new System.Drawing.Size(60, 22);
            this.btnNewNote.Text = "新增日记";
            this.btnNewNote.Click += new System.EventHandler(this.btnNewNote_Click);
            // 
            // btnDelNote
            // 
            this.btnDelNote.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btnDelNote.Image = ((System.Drawing.Image)(resources.GetObject("btnDelNote.Image")));
            this.btnDelNote.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnDelNote.Name = "btnDelNote";
            this.btnDelNote.Size = new System.Drawing.Size(60, 22);
            this.btnDelNote.Text = "删除日记";
            this.btnDelNote.Click += new System.EventHandler(this.btnDelNote_Click);
            // 
            // splitMain
            // 
            this.splitMain.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.splitMain.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitMain.Location = new System.Drawing.Point(0, 25);
            this.splitMain.Name = "splitMain";
            this.splitMain.Size = new System.Drawing.Size(424, 236);
            this.splitMain.SplitterDistance = 141;
            this.splitMain.TabIndex = 5;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(424, 261);
            this.Controls.Add(this.splitMain);
            this.Controls.Add(this.toolStrip1);
            this.Name = "MainForm";
            this.Text = "测试嵌入两个浏览器";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitMain)).EndInit();
            this.splitMain.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton btnNewNote;
        private System.Windows.Forms.ToolStripButton btnDelNote;
        private System.Windows.Forms.SplitContainer splitMain;
    }
}

