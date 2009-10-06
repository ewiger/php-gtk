--TEST--
GtkTreeView->convert_tree_to_bin_window_coords method
--SKIPIF--
<?php
if(!extension_loaded('php-gtk')) die('skip - PHP-GTK extension not available');
if(!method_exists('GtkTreeView', 'convert_tree_to_bin_window_coords')) die('skip - GtkTreeView->convert_tree_to_bin_window_coords not available, requires GTK 2.12 or higher');
?>
--FILE--
<?php
$treeview = new GtkTreeView();
var_dump($treeview->convert_tree_to_bin_window_coords(0, 0));

/* Wrong number args */
$treeview->convert_tree_to_bin_window_coords();
$treeview->convert_tree_to_bin_window_coords(1);
$treeview->convert_tree_to_bin_window_coords(1, 1, 1);

/* Arg type checks */
$treeview->convert_tree_to_bin_window_coords(array(), 1);
$treeview->convert_tree_to_bin_window_coords(1, array());
?>
--EXPECTF--
array(2) {
  [0]=>
  int(%i)
  [1]=>
  int(%i)
}

Warning: GtkTreeView::convert_tree_to_bin_window_coords() requires exactly 2 arguments, 0 given in %s on line %d

Warning: GtkTreeView::convert_tree_to_bin_window_coords() requires exactly 2 arguments, 1 given in %s on line %d

Warning: GtkTreeView::convert_tree_to_bin_window_coords() requires exactly 2 arguments, 3 given in %s on line %d

Warning: GtkTreeView::convert_tree_to_bin_window_coords() expects argument 1 to be integer, array given in %s on line %d

Warning: GtkTreeView::convert_tree_to_bin_window_coords() expects argument 2 to be integer, array given in %s on line %d