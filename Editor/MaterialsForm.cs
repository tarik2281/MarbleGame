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
    public partial class MaterialsForm : Form
    {
        List<External.EditorMaterial> m_materials;

        public MaterialsForm()
        {
            InitializeComponent();
        }

        private void MaterialsForm_Load(object sender, EventArgs e)
        {
            m_materials = new List<External.EditorMaterial>();
            pictureBox1.SizeMode = PictureBoxSizeMode.StretchImage;
            pictureBox2.SizeMode = PictureBoxSizeMode.StretchImage;

            External.EditorMaterial mat = new External.EditorMaterial();
            bool result = External.MaterialsBegin(out mat);
            while (result == true)
            {
                string item = Marshal.PtrToStringAnsi(mat.name);
                listBox1.Items.Add(item);
                string diffuseMap = Marshal.PtrToStringAnsi(mat.diffuseMap);
                string normalMap = Marshal.PtrToStringAnsi(mat.normalMap);
                mat.diffuseImage = Image.FromFile("Resources\\" + diffuseMap);
                mat.normalImage = Image.FromFile("Resources\\" + normalMap);
                m_materials.Add(mat);
                result = External.MaterialsNext(out mat);
            }
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex > listBox1.Items.Count || listBox1.SelectedIndex < 0)
                return;
            string diffuseMap = Marshal.PtrToStringAnsi(m_materials[listBox1.SelectedIndex].diffuseMap);
            textBox1.Text = diffuseMap;
            textBox2.Text = Marshal.PtrToStringAnsi(m_materials[listBox1.SelectedIndex].name);
            textBox3.Text = Marshal.PtrToStringAnsi(m_materials[listBox1.SelectedIndex].normalMap);
            pictureBox1.Image = m_materials[listBox1.SelectedIndex].diffuseImage;
            pictureBox2.Image = m_materials[listBox1.SelectedIndex].normalImage;
        }

        private void toolStripButton2_Click(object sender, EventArgs e)
        {
            if (listBox1.SelectedIndex < 0 || listBox1.SelectedIndex > listBox1.Items.Count)
                return;

            m_materials.RemoveAt(listBox1.SelectedIndex);
            listBox1.Items.RemoveAt(listBox1.SelectedIndex);
        }
    }
}
