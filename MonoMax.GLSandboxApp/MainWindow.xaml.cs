using System.Windows;

namespace MonoMax.GLSandboxApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void OnClicked(object sender, RoutedEventArgs e)
        {
            var actorWindow = new ActorEditorWindow();
            actorWindow.Show();
        }

        private void OnKeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
        }

        private void OnKeyUp(object sender, System.Windows.Input.KeyEventArgs e)
        {

        }

        private void OnLButtonDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {

        }

        private void OnLButtonUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {

        }

        private void OnMouseWheel(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {

        }
    }
}
