using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace Editor
{
    public partial class Form1 : Form
    {
        bool changePosition;
        MessageHandler handler;
        string path;
        bool changeObject;
        string oldObj;

        public Form1()
        {
            InitializeComponent();
            changePosition = false;
            changeObject = false;
            handler = new MessageHandler(this.Handle, this.panel1.Handle, this);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            External.CreateView(this.panel1.Handle);
            this.MouseWheel += Form1_MouseWheel;

            External.EditorMaterial mat = new External.EditorMaterial();
            bool result = External.MaterialsBegin(out mat);
            while (result == true)
            {
                string item = Marshal.PtrToStringAnsi(mat.name);
                comboBox1.Items.Add(item);
                result = External.MaterialsNext(out mat);
            }
        }

        void Form1_MouseWheel(object sender, MouseEventArgs e)
        {
            byte modifier = 0x0;
            if ((ModifierKeys & Keys.Shift) == Keys.Shift)
            {
                modifier |= External.MK_SHIFT;
            }
            if ((ModifierKeys & Keys.Control) == Keys.Control)
            {
                modifier |= External.MK_CONTROL;
            }
            External.WndProc(panel1.Handle, 0x020A, (int)((e.Delta<<16)) | modifier, 0);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            
        }

        public MessageHandler GetMessageHandler()
        {
            return handler;
        }

        struct Object
        {
            public string name;
            public string parentName;
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                path = openFileDialog1.FileName;
                this.Text = "MarbleGame Editor - " + path;
                External.LoadLevel(openFileDialog1.FileName);
                treeView1.Nodes.Clear();
                IntPtr namePtr;
                bool result = External.ObjectsBegin(out namePtr);
                Queue<Object> objectQueue = new Queue<Object>();
                while (result)
                {
                    string name = Marshal.PtrToStringAnsi(namePtr);
                    External.ObjectData data = new External.ObjectData();
                    External.GetObjectDataByName(name, out data);
                    string parentName = Marshal.PtrToStringAnsi(data.parent);
                    if (parentName.CompareTo("None") != 0)
                    {
                        TreeNode[] nodes = treeView1.Nodes.Find(parentName, false);
                        if (nodes.Count() > 0)
                            nodes.First().Nodes.Add(name, name);
                        else
                        {
                            Object obj;
                            obj.name = name;
                            obj.parentName = parentName;
                            objectQueue.Enqueue(obj);
                        }
                    }
                    else
                    {
                        treeView1.Nodes.Add(name, name);
                    }
                    result = External.ObjectsNext(out namePtr);
                }

                while (objectQueue.Count > 0)
                {
                    Object obj = objectQueue.Peek();
                    TreeNode[] nodes = treeView1.Nodes.Find(obj.parentName, false);
                    if (nodes.Count() > 0)
                        nodes.First().Nodes.Add(obj.name, obj.name);
                    objectQueue.Dequeue();
                }
            }
        }

        private void panel1_MouseClick(object sender, MouseEventArgs e)
        {

        }

        public void SetText(string gameObject)
        {
            changeObject = true;
            textBox1.Text = gameObject;
            oldObj = gameObject;
            changeObject = false;
        }

        public string GetObject()
        {
            return textBox1.Text;
        }

        public void SetObjectType(char flag)
        {
            switch (flag)
            {
                case 'w':
                    textBox2.Text = "Water";
                    break;
                case 'l':
                    textBox2.Text = "Terrain";
                    break;
                case 's':
                    textBox2.Text = "Star";
                    break;
                case 'b':
                    textBox2.Text = "Button";
                    break;
                case 'r':
                    textBox2.Text = "Ramp";
                    break;
                case 'o':
                    textBox2.Text = "Platform";
                    break;
                case 't':
                    textBox2.Text = "Teleporter";
                    break;
                case 'p':
                    textBox2.Text = "PowerUp";
                    break;
                default:
                    break;
            }
        }

        public External.Vector3 GetPosition()
        {
            External.Vector3 pos = new External.Vector3();
            pos.x = float.Parse(textBox3.Text);
            pos.y = float.Parse(textBox4.Text);
            pos.z = float.Parse(textBox5.Text);
            return pos;
        }

        public void SetPosition(External.Vector3 position)
        {
            changePosition = false;
            textBox3.Text = position.x.ToString();
            textBox4.Text = position.y.ToString();
            textBox5.Text = position.z.ToString();
            changePosition = true;
        }

        public void SetSize(External.Vector3 size)
        {
            maskedTextBox1.Text = size.x.ToString();
            maskedTextBox2.Text = size.y.ToString();
            maskedTextBox3.Text = size.z.ToString();
        }

        public void SetSizesVisible(bool visible)
        {
            groupBox1.Visible = visible;
        }

        public void SetTeleporterVisible(bool visible)
        {
            groupBox2.Visible = visible;
        }

        public void SetTarget(string target)
        {
            textBox6.Text = target;
        }

        private void materialsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            new MaterialsForm().Show();
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                External.SaveLevel(saveFileDialog1.FileName);
            }
        }

        private void textBox4_TextChanged(object sender, EventArgs e)
        {
            if (changePosition)
            {
                try
                {
                    External.Vector3 pos = new External.Vector3();
                    pos.x = float.Parse(textBox3.Text);
                    pos.y = float.Parse(textBox4.Text);
                    pos.z = float.Parse(textBox5.Text);
                    External.SetPositionForObject(GetObject(), pos);
                }
                catch (System.FormatException ex)
                {

                }
            }
        }

        private void uncheckItems(object sender)
        {
            External.RemoveTemp();
            foreach (ToolStripItem item in toolStrip1.Items)
            {
                if (item == sender)
                    continue;
                if (item.GetType() == typeof(ToolStripButton))
                {
                    ((ToolStripButton)item).Checked = false;
                }
            }
        }

        private void toolStripButton1_Click(object sender, EventArgs e)
        {
            uncheckItems(sender);
            if (toolStripButton1.Checked)
                External.AddTemporaryObject('s');
            handler.m_resizing = false;
            handler.m_addObject = (toolStripButton1.Checked) ? 's' : '\0';
        }

        private void toolStripButton2_Click(object sender, EventArgs e)
        {
            string obj = GetObject();
            if (obj.CompareTo("Respawn") != 0)
            {
                External.RemoveObjectByName(obj);
                TreeNode node = treeView1.Nodes.Find(obj, true).First();
                node.Remove();
            }
        }

        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {
               
        }

        private void panel1_Click(object sender, EventArgs e)
        {

        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            External.SaveLevel(path);
        }

        private void toolStripButton3_Click(object sender, EventArgs e)
        {
            uncheckItems(sender);
            if (toolStripButton3.Checked)
                External.AddTemporaryObject('b');
            handler.m_resizing = false;
            handler.m_addObject = (toolStripButton3.Checked) ? 'b' : '\0';
        }

        private void toolStripButton4_Click(object sender, EventArgs e)
        {
            uncheckItems(sender);
            handler.m_resizing = toolStripButton4.Checked;
            External.SetPlatformResizing(toolStripButton4.Checked);
        }

        private void toolStripButton5_Click(object sender, EventArgs e)
        {
            uncheckItems(sender);
            if (toolStripButton5.Checked)
                External.AddTemporaryObject('o');
            handler.m_resizing = false;
            handler.m_addObject = (toolStripButton5.Checked) ? 'o' : '\0';
        }

        private void panel1_Resize(object sender, EventArgs e)
        {
            External.ResizeView(panel1.Width, panel1.Height);
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            string gameObject = GetObject();
            if (comboBox1.SelectedIndex >= 0 && comboBox1.SelectedIndex < comboBox1.Items.Count && gameObject.Length > 0)
            {
                string material = comboBox1.Items[comboBox1.SelectedIndex].ToString();
                External.SetPlatformMaterial(gameObject, material);
            }
        }

        private void toolStripButton6_Click(object sender, EventArgs e)
        {
            uncheckItems(sender);
            if (toolStripButton6.Checked)
                External.AddTemporaryObject('r');
            handler.m_resizing = false;
            handler.m_addObject = (toolStripButton6.Checked) ? 'r' : '\0';
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            if (!changeObject)
            {
                
            }
        }

        private void textBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar == (char)Keys.Enter)
            {
                External.SetNameForObject(oldObj, GetObject());
                TreeNode node = treeView1.Nodes.Find(oldObj, false).First();
                node.Name = GetObject();
                node.Text = GetObject();
            }
        }

        private void toolStripButton7_Click(object sender, EventArgs e)
        {
            uncheckItems(sender);
            if (toolStripButton7.Checked)
                External.AddTemporaryObject('t');
            handler.m_resizing = false;
            handler.m_addObject = (toolStripButton7.Checked) ? 't' : '\0';
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            handler.m_selectTeleport = true;
        }

        private void toolStripButton8_Click(object sender, EventArgs e)
        {
            uncheckItems(sender);
            if (toolStripButton8.Checked)
                External.AddTemporaryObject('p');
            handler.m_resizing = false;
            handler.m_addObject = (toolStripButton8.Checked) ? 'p' : '\0';
        }

        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            External.NewLevel();
            treeView1.Nodes.Clear();
        }

        private void rotatePlatformToolStripMenuItem_Click(object sender, EventArgs e)
        {
            External.AddRotationForPlatform(GetObject(), 1);
        }

        private void toolStripButton9_Click(object sender, EventArgs e)
        {
            uncheckItems(sender);
            if (toolStripButton9.Checked)
                External.AddTemporaryObject('c');
            handler.m_resizing = false;
            handler.m_addObject = (toolStripButton9.Checked) ? 'c' : '\0';
        }
    }

    public class External
    {
        public const int MK_CONTROL = 0x0008;
        public const int MK_SHIFT = 0x0004;

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool CreateView(IntPtr hwnd);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void RenderView();

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ReleaseView();

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ResizeView(int width, int height);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void NewLevel();

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void LoadLevel(string path);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SaveLevel(string path);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)] 
        public static extern void WndProc(IntPtr hwnd, int msg, int wParam, int lParam);

        [DllImport("MarbleGame.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr PickObject();

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void PickTempObject();


        public struct EditorMaterial
        {
            public IntPtr name;
            public IntPtr diffuseMap;
            public IntPtr normalMap;
            public Image diffuseImage;
            public Image normalImage;
        }

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        [return:MarshalAs(UnmanagedType.I1)]
        public static extern bool MaterialsBegin(out EditorMaterial mat);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        [return:MarshalAs(UnmanagedType.I1)]
        public static extern bool MaterialsNext(out EditorMaterial mat);


        public struct Vector3
        {
            public float x, y, z;

            public static Vector3 operator +(Vector3 v1, Vector3 v2)
            {
                Vector3 temp;
                temp.x = v1.x + v2.x;
                temp.y = v1.y + v2.y;
                temp.z = v1.z + v2.z;
                return temp;
            }

            public static Vector3 operator *(Vector3 v, float f)
            {
                Vector3 temp;
                temp.x = v.x * f;
                temp.y = v.y * f;
                temp.z = v.z * f;
                return temp;
            }
        }

        public struct Quaternion
        {
            public float x, y, z, w;

            public Quaternion(float x, float y, float z, float angle)
            {
                float rad = angle / 2.0f * (float)Math.PI / 180.0f;
                float sin = (float)Math.Sin(rad);
                this.x = sin * x;
                this.y = sin * y;
                this.z = sin * z;
                this.w = (float)Math.Cos(rad);
            }
        }

        public struct ObjectData
        {
            public char flag;
            public Vector3 position;
            public Vector3 rotation;
            public IntPtr parent;
        }
        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetObjectDataByName(string name, out ObjectData data);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetPositionForObject(string name, Vector3 position);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void AddRotationForObject(string name, Quaternion rotation);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void AddRotationForPlatform(string name, int direction);

        public struct Ray
        {
            public Vector3 position;
            public Vector3 direction;
        }

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetMouseRay(out Ray ray);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetPositionOnPlane(Ray ray, float y, out Vector3 position);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetPositionOnCamPlane(Ray ray, Vector3 pos, out Vector3 position);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetWorldPosition(Ray ray, out Vector3 position);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr AddObject(Vector3 position, char flag, string parent);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void RemoveObjectByName(string name);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetPlatformResizing(bool resizing);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        [return:MarshalAs(UnmanagedType.I1)]
        public static extern bool ObjectsBegin(out IntPtr name);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool ObjectsNext(out IntPtr name);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetPlatformData(string name, out Vector3 size);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetPlatformMaterial(string name, string material);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetNameForObject(string name, string newName);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetTargetForTeleporter(string name, string target);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetTeleporterData(string name, out IntPtr target);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void AddTemporaryObject(char flag);

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void RemoveTemp();

        [DllImport("MarbleGame.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetTempPosition(Vector3 position);
    }
}
