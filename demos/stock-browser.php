<?php

class StockItemInfo {
	var $stock_id = '';
	var $stock_item = null;
	var $small_icon = null;
	var $macro = '';
	var $accel_str = '';

	function __construct($stock_id = null) {
		$this->stock_id = $stock_id;
		if ($stock_id) {
			$this->macro = id_to_macro($stock_id);
		}
	}
}

class StockItemBrowserDemo extends GtkWindow {
	function __construct($parent = null)
	{
		parent::__construct();

		if ($parent)
			$this->set_screen($parent->get_screen());
		else
			$this->connect_object('destroy', array('gtk', 'main_quit'));

		$this->set_title(__CLASS__);
		$this->set_default_size(-1, 500);
		$this->set_border_width(8);

		$hbox = new GtkHBox(false, 8);
		$this->add($hbox);

		$scrolled = new GtkScrolledWindow();
		$scrolled->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
		$scrolled->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
		$hbox->pack_start($scrolled, false, false, 0);

		$model = $this->create_model();
		$treeview = new GtkTreeView($model);
		$scrolled->add($treeview);

		$column = new GtkTreeViewColumn();

		$this->show_all();
	}

	private function create_model()
	{
		$store = new GtkListStore(Gtk::TYPE_PHP_VALUE, Gtk::TYPE_STRING);

		$ids = Gtk::stock_list_ids();
		sort($ids);

		foreach ($ids as $id) {
			$info = new StockItemInfo($id);
			$stock_item = Gtk::stock_lookup($id);
			if ($stock_item)
				$info->stock_item = $stock_item;
			else
				$info->stock_item = array('', '', 0, 0, '');

			$icon_set = GtkIconFactory::lookup_default($id);
			if ($icon_set) {
				$sizes = $icon_set->get_sizes();
				$size = $sizes[0];
				for ($i = 0; $i < count($sizes); $i++) {
					if ($sizes[$i] == Gtk::ICON_SIZE_MENU) {
						$size = Gtk::ICON_SIZE_MENU;
						break;
					}
				}
				$info->small_icon = $this->render_icon($info->stock_id, $size);
				if ($size != Gtk::ICON_SIZE_MENU) {
					list($width, $height) = Gtk::icon_size_lookup(Gtk::ICON_SIZE_MENU);

					$info->small_icon = $info->small_icon->scale_simple($width, $height, 'bilinear');
				}

			} else {
				$info->small_icon = null;
			}

			if ($info->stock_item[3] == 0) {
				$info->accel_str = '';
			} else {
				$info->accel_str = Gtk::accelerator_get_label($info->stock_item[3], $info->stock_item[2]);
			}

			$iter = $store->append();
			$store->set($iter, 0, $info, 1, $id);
		}
	}
}

function id_to_macro($id)
{
	if (substr($id, 0, 3) == 'gtk') {
		$macro = 'Gtk::STOCK' . preg_replace('!-([^-]+)!e', '"_".strtoupper("$1")', substr($id, 3));
	} else {
		$macro = substr(preg_replace('!([^-]+)-?!e', 'strtoupper($1)', $id), 1);
	}

	return $macro;
}

new StockItemBrowserDemo();

Gtk::main();

?>
