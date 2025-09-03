using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace AxoSlicer_Ui.Commands
{
    internal class RelayCommand : ICommand
    {
        private readonly Action<object?> _execute;
        private readonly Func<object?, bool> _canExecute;

        public event EventHandler? CanExecuteChanged;

        public RelayCommand( Action<object?> execute, Func<object?, bool>  canExecute = null )
        {
            _execute = execute;
            _canExecute = canExecute;
        }

        public bool CanExecute(object? paraemter) => _canExecute?.Invoke( paraemter)?? true;
        public void Execute(object? parameter) => _execute?.Invoke( parameter);
        public void RaiseCanExecuteChanged() => CanExecuteChanged?.Invoke(this, EventArgs.Empty);
    }
}
