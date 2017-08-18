using System;
using System.Diagnostics;
using System.IO.Ports;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;

namespace ArduinoSerial
{
    internal static class Program
    {
        private const int WhKeyboardLl = 13;
        private const int WmKeydown = 0x0100;
        private static readonly LowLevelKeyboardProc Proc = HookCallback;
        private static IntPtr _hookId = IntPtr.Zero;
        private static SerialPort _currentPort;

        public static void Main()
        {
            if (!SetComPort())
            {
                Console.WriteLine("Can't find Arduino, exiting.");
                Console.ReadKey(true);
                Application.Exit();
            }

            Console.WriteLine($"Found Arduino on: {_currentPort.PortName}");

            _hookId = SetHook(Proc);

            Application.Run();

            UnhookWindowsHookEx(_hookId);
        }

        private static bool SetComPort()
        {
            var ports = SerialPort.GetPortNames();
            foreach (var port in ports)
            {
                _currentPort = new SerialPort(port, 115200);
                if (DetectArduino())
                    return true;
            }

            return false;
        }

        private static bool DetectArduino()
        {
            try
            {
                _currentPort.Open();
                _currentPort.Write(new[] {Convert.ToByte(16)}, 0, 1);
                Thread.Sleep(200);
                var count = _currentPort.BytesToRead;
                var returnMessage = string.Empty;
                while (count > 0)
                {
                    returnMessage = returnMessage + Convert.ToChar(_currentPort.ReadByte());
                    count--;
                }

                _currentPort.Close();
                return returnMessage.Contains("hello");
            }
            catch
            {
                return false;
            }
        }

        private static IntPtr SetHook(LowLevelKeyboardProc proc)
        {
            using (var curProcess = Process.GetCurrentProcess())
            using (var curModule = curProcess.MainModule)
            {
                return SetWindowsHookEx(WhKeyboardLl, proc,
                    GetModuleHandle(curModule.ModuleName), 0);
            }
        }


        private static IntPtr HookCallback(
            int nCode, IntPtr wParam, IntPtr lParam)
        {
            if (nCode >= 0 && wParam == (IntPtr) WmKeydown)
                // ReSharper disable once SwitchStatementMissingSomeCases
                switch ((Keys) Marshal.ReadInt32(lParam))
                {
                    case Keys.F6:
                        Send(50);
                        break;
                    case Keys.F7:
                        Send(49);
                        break;
                    case Keys.F8:
                        Send(48);
                        break;
                }

            return CallNextHookEx(_hookId, nCode, wParam, lParam);
        }

        private static void Send(int data)
        {
            _currentPort.Open();
            _currentPort.Write(new[] { Convert.ToByte(data) }, 0, 1);
            _currentPort.Close();
        }


        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr SetWindowsHookEx(int idHook,
            LowLevelKeyboardProc lpfn, IntPtr hMod, uint dwThreadId);


        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool UnhookWindowsHookEx(IntPtr hhk);


        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr CallNextHookEx(IntPtr hhk, int nCode,
            IntPtr wParam, IntPtr lParam);


        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr GetModuleHandle(string lpModuleName);


        private delegate IntPtr LowLevelKeyboardProc(
            int nCode, IntPtr wParam, IntPtr lParam);
    }
}