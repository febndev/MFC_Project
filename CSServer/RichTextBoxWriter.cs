using System;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace CSServer
{
    public class RichTextBoxWriter : TextWriter
    {

        private readonly RichTextBox _box;
        public RichTextBoxWriter(RichTextBox box) => _box = box;

        public override Encoding Encoding => Encoding.UTF8;

        public override void Write(string? value) => Append(value ?? "");
        public override void WriteLine(string? value) => Append((value ?? "") + Environment.NewLine);

        private void Append(string text)
        {
            if (_box.IsDisposed) return;

            if (_box.InvokeRequired)
            {
                _box.BeginInvoke(new Action<string>(Append), text);
                return;
            }

            _box.AppendText(text);
            // 자동 스크롤
            _box.SelectionStart = _box.TextLength;
            _box.ScrollToCaret();
        }
    }
}
