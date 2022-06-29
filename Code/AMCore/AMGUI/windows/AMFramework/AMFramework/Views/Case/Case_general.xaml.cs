﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace AMFramework.Views.Case
{
    /// <summary>
    /// Interaction logic for Case_general.xaml
    /// </summary>
    public partial class Case_general : UserControl
    {
        public Case_general()
        {
            InitializeComponent();
        }

        public Case_general(Controller.Controller_DBS_Projects projectController)
        {
            InitializeComponent();
            DataContext = projectController;
        }
    }
}
