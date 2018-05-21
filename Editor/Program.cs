using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace Editor
{
    public class MessageHandler : IMessageFilter
    {
        const int WM_MOUSEMOVE = 0x0200;
        const int WM_LBUTTONDOWN = 0x0201;
        const int WM_LBUTTONUP = 0x0202;
        const int WM_LBUTTONDBLCLK = 0x0203;
        const int WM_RBUTTONDOWN = 0x0204;
        const int WM_RBUTTONUP = 0x0205;
        const int WM_RBUTTONDBLCLK = 0x0206;
        const int WM_MBUTTONDOWN = 0x0207;
        const int WM_MBUTTONUP = 0x0208;
        const int WM_MBUTTONDBLCLK = 0x0209;
        const int WM_MOUSEWHEEL = 0x020A;

        const int WM_KEYDOWN = 0x0100;
        const int WM_KEYUP = 0x0101;
        const int WM_SYSKEYDOWN = 0x0104;
        const int WM_SYSKEYUP = 0x0105;
        const int WM_CLOSE = 0x0010;

        IntPtr m_formHandle;
        IntPtr m_displayPanelHandle;
        Form1 m_parent;
        bool m_gameIsRunning;
        bool m_holding;
        bool m_moving;
        bool m_yAxis;
        External.Vector3 oldPosition;
        public char m_addObject;
        public bool m_resizing;
        bool m_rotating;
        int oldX, oldY;
        public bool m_selectTeleport;

        public MessageHandler(IntPtr formHandle, IntPtr displayPanelHandle, Form1 parent)
        {
            m_formHandle = formHandle;
            m_displayPanelHandle = displayPanelHandle;
            m_parent = parent;
            m_gameIsRunning = false;
            m_holding = false;
            m_moving = false;
            m_yAxis = false;
            m_resizing = false;
            m_rotating = false;
            m_selectTeleport = false;
            m_addObject = '\0';
        }

        public bool PreFilterMessage(ref Message m)
        {


            if (m.HWnd == m_displayPanelHandle)
            {
                switch (m.Msg)
                {
                    case WM_MOUSEWHEEL:
                        External.WndProc(m_displayPanelHandle, m.Msg, m.WParam.ToInt32(), m.LParam.ToInt32());
                        return true;

                    case WM_LBUTTONDOWN:
                        {
                            if (!m_resizing)
                            {
                                if (m_addObject == '\0')
                                {
                                    string oldObject = m_parent.GetObject();
                                    string gameObject = Marshal.PtrToStringAnsi(External.PickObject());
                                    m_parent.SetText(gameObject);
                                    if (gameObject.Length > 0)
                                    {
                                        External.ObjectData data = new External.ObjectData();
                                        External.GetObjectDataByName(gameObject, out data);
                                        m_parent.SetPosition(data.position);
                                        m_parent.SetObjectType(data.flag);
                                        oldPosition = data.position;
                                        m_holding = true;
                                        m_yAxis = ((m.WParam.ToInt32() & 0x0004) == 0x0004);
                                        m_rotating = ((m.WParam.ToInt32() & 0x0008) == 0x0008);
                                        if (data.flag == 'o' || data.flag == 'r')
                                        {
                                            External.Vector3 size = new External.Vector3();
                                            External.GetPlatformData(gameObject, out size);
                                            m_parent.SetSize(size);
                                            m_parent.SetSizesVisible(true);
                                        }
                                        else
                                        {
                                            m_parent.SetSizesVisible(false);
                                        }

                                        if (data.flag == 't') {
                                            IntPtr targetPtr;
                                            External.GetTeleporterData(gameObject, out targetPtr);
                                            string target = Marshal.PtrToStringAnsi(targetPtr);
                                            m_parent.SetTarget(target);
                                            m_parent.SetTeleporterVisible(true);
                                        } else {
                                            m_parent.SetTeleporterVisible(false);
                                        }

                                        if (m_selectTeleport && data.flag == 't')
                                        {
                                            External.SetTargetForTeleporter(oldObject, gameObject);
                                            m_selectTeleport = false;
                                        }
                                    }
                                }
                                else
                                {
                                    External.Ray mouseRay = new External.Ray();
                                    External.GetMouseRay(out mouseRay);
                                    External.Vector3 worldPos = new External.Vector3();
                                    string parentObject = Marshal.PtrToStringAnsi(External.GetWorldPosition(mouseRay, out worldPos));
                                    worldPos.y += 1.0f;
                                    IntPtr name = External.AddObject(worldPos, m_addObject, parentObject);
                                    string objectName = Marshal.PtrToStringAnsi(name);
                                    if (m_addObject == 'b' && parentObject.CompareTo("None") != 0)
                                    {
                                        TreeNode node = m_parent.treeView1.Nodes.Find(parentObject, false).First();
                                        node.Nodes.Add(objectName, objectName);
                                    }
                                    else
                                    {
                                        m_parent.treeView1.Nodes.Add(objectName, objectName);
                                    }
                                }
                            }
                            External.WndProc(m_displayPanelHandle, m.Msg, m.WParam.ToInt32(), m.LParam.ToInt32());
                            return true;
                        }
                    case WM_MOUSEMOVE:
                        {
                            int curX = Cursor.Position.X;
                            int curY = Cursor.Position.Y;
                            int deltaX = curX - oldX;
                            int deltaY = curY - oldY;
                            if (m_holding && (Math.Abs(deltaX) > 2 || Math.Abs(deltaY) > 2))
                                m_moving = true;
                            string gameObject = m_parent.GetObject();
                            if (gameObject.Length > 0 && m_moving)
                            {
                                External.Ray mouseRay = new External.Ray();
                                External.GetMouseRay(out mouseRay);
                                External.Vector3 y = m_parent.GetPosition();
                                if (m_yAxis)
                                {
                                    External.Vector3 position = new External.Vector3();
                                    External.GetPositionOnCamPlane(mouseRay, oldPosition, out position);
                                    position.x = oldPosition.x;
                                    position.z = oldPosition.z;
                                    External.SetPositionForObject(gameObject, position);
                                    m_parent.SetPosition(position);
                                }
                                else if (m_rotating)
                                {
                                    External.Quaternion q = new External.Quaternion(0.0f, 1.0f, 0.0f, (float)deltaX * 0.5f);
                                    External.AddRotationForObject(gameObject, q);
                                }
                                else
                                {
                                    External.Vector3 position = new External.Vector3();
                                    External.GetPositionOnPlane(mouseRay, y.y, out position);
                                    position.y = y.y;
                                    External.SetPositionForObject(gameObject, position);
                                    m_parent.SetPosition(position);
                                }
                                
                            }

                            if (m_addObject != '\0')
                            {
                                External.Ray ray = new External.Ray();
                                External.Vector3 pos = new External.Vector3();
                                External.GetMouseRay(out ray);
                                External.GetWorldPosition(ray, out pos);
                                External.SetTempPosition(pos);
                            }
                            else if (!m_moving)
                            {
                                External.PickTempObject();
                            }

                            oldX = curX;
                            oldY = curY;
                            External.WndProc(m_displayPanelHandle, m.Msg, m.WParam.ToInt32(), m.LParam.ToInt32());
                            return true;
                        }
                    case WM_LBUTTONUP:
                        {
                            m_holding = false;
                            m_moving = false;
                            External.WndProc(m_displayPanelHandle, m.Msg, m.WParam.ToInt32(), m.LParam.ToInt32());
                            return true;
                        }
                    case WM_LBUTTONDBLCLK:
                    case WM_RBUTTONDOWN:
                    case WM_RBUTTONUP:
                    case WM_RBUTTONDBLCLK:
                    case WM_MBUTTONDOWN:
                    case WM_MBUTTONUP:
                    case WM_MBUTTONDBLCLK:
                    case WM_KEYDOWN:
                        {
                            if (m.WParam.ToInt32() == 0x08)
                                External.RemoveObjectByName(m_parent.GetObject());
                            External.WndProc(m_displayPanelHandle, m.Msg, m.WParam.ToInt32(), m.LParam.ToInt32());
                            return true;
                        }
                    case WM_KEYUP:
                    case WM_SYSKEYDOWN:
                    case WM_SYSKEYUP:
                    case WM_CLOSE:
                        External.WndProc(m_displayPanelHandle, m.Msg, m.WParam.ToInt32(), m.LParam.ToInt32());
                        return true;
                }
            }

            return false;
        }

        public void Application_Idle(object sender, EventArgs e)
        {
            External.RenderView();

            m_parent.Invalidate();
        }
    }

    static class Program
    {
        static Form1 m_form;

        /// <summary>
        /// Der Haupteinstiegspunkt für die Anwendung.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            m_form = new Form1();
            MessageHandler messageHandler = m_form.GetMessageHandler();
            Application.AddMessageFilter(messageHandler);
            Application.Idle += new EventHandler(messageHandler.Application_Idle);
            Application.Run(m_form);
        }

        static void Application_Idle(object sender, EventArgs e)
        {
            External.RenderView();
        }
    }
}
