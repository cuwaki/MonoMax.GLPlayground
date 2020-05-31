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
    }
}
