<?php
// Call GtkFileFilterTest::main() if this source file is executed directly.
if (!defined("PHPUnit_MAIN_METHOD")) {
    define("PHPUnit_MAIN_METHOD", "GtkFileFilterTest::main");
}

require_once "PHPUnit/Framework/TestCase.php";
require_once "PHPUnit/Framework/TestSuite.php";

// You may remove the following line when all tests have been implemented.
require_once "PHPUnit/Framework/IncompleteTestError.php";



/**
 * Test class for GtkFileFilter.
 * Generated by PHPUnit2_Util_Skeleton on 2006-03-07 at 13:26:41.
 */
class GtkFileFilterTest extends PHPUnit_Framework_TestCase {
    /**
     * Runs the test methods of this class.
     *
     * @access public
     * @static
     */
    public static function main() {
        require_once "PHPUnit/TextUI/TestRunner.php";

        $suite  = new PHPUnit_Framework_TestSuite("GtkFileFilterTest");
        $result = PHPUnit_TextUI_TestRunner::run($suite);
    }

    /**
     * Sets up the fixture, for example, open a network connection.
     * This method is called before a test is executed.
     *
     * @access protected
     */
    protected function setUp() {
    }

    /**
     * Tears down the fixture, for example, close a network connection.
     * This method is called after a test is executed.
     *
     * @access protected
     */
    protected function tearDown() {
    }

    public function testAdd_custom() {
        $info = array(
            '/data/test.txt',
            'file:///data/test.txt',
            'test.txt',
            'text/plain'
        );

        $filter = new GtkFileFilter();
        $filter->add_custom(
            Gtk::FILE_FILTER_FILENAME
            | Gtk::FILE_FILTER_URI
            | Gtk::FILE_FILTER_DISPLAY_NAME
            | Gtk::FILE_FILTER_MIME_TYPE,
            array($this, 'callback_add_custom')
        );

        $this->called = false;
        $this->assertTrue($filter->filter($info));
        $this->assertTrue($this->called);
    }

    public function callback_add_custom($info) {
        $this->called = true;
        return true;
    }

    /**
     * @todo Implement testAdd_mime_type().
     */
    public function testAdd_mime_type() {
        // Remove the following line when you implement this test.
        throw new PHPUnit_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testAdd_pattern().
     */
    public function testAdd_pattern() {
        // Remove the following line when you implement this test.
        throw new PHPUnit_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testAdd_pixbuf_formats().
     */
    public function testAdd_pixbuf_formats() {
        // Remove the following line when you implement this test.
        throw new PHPUnit_Framework_IncompleteTestError;
    }

    public function testFilter() {
        $info = array(
            '/data/test.txt',
            'file:///data/test.txt',
            'test.txt',
            'text/plain'
        );

        $filter = new GtkFileFilter();
        $filter->add_pattern('*.txt');
        $this->assertTrue($filter->filter($info));

        $filter = new GtkFileFilter();
        $filter->add_pattern('*.csv');
        $this->assertFalse($filter->filter($info));

        $filter = new GtkFileFilter();
        $filter->add_mime_type('text/plain');
        $this->assertTrue($filter->filter($info));

        $filter = new GtkFileFilter();
        $filter->add_mime_type('text/csv');
        $this->assertFalse($filter->filter($info));
    }

    /**
     * @todo Implement testGet_name().
     */
    public function testGet_name() {
        // Remove the following line when you implement this test.
        throw new PHPUnit_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testGet_needed().
     */
    public function testGet_needed() {
        // Remove the following line when you implement this test.
        throw new PHPUnit_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testSet_name().
     */
    public function testSet_name() {
        // Remove the following line when you implement this test.
        throw new PHPUnit_Framework_IncompleteTestError;
    }
}

// Call GtkFileFilterTest::main() if this source file is executed directly.
if (PHPUnit_MAIN_METHOD == "GtkFileFilterTest::main") {
    GtkFileFilterTest::main();
}
?>
