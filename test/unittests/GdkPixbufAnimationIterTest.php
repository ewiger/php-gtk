<?php
// Call GdkPixbufAnimationIterTest::main() if this source file is executed directly.
if (!defined("PHPUnit2_MAIN_METHOD")) {
    define("PHPUnit2_MAIN_METHOD", "GdkPixbufAnimationIterTest::main");
}

require_once "PHPUnit2/Framework/TestCase.php";
require_once "PHPUnit2/Framework/TestSuite.php";

// You may remove the following line when all tests have been implemented.
require_once "PHPUnit2/Framework/IncompleteTestError.php";



/**
 * Test class for GdkPixbufAnimationIter.
 * Generated by PHPUnit2_Util_Skeleton on 2006-03-07 at 13:26:40.
 */
class GdkPixbufAnimationIterTest extends PHPUnit2_Framework_TestCase {
    /**
     * Runs the test methods of this class.
     *
     * @access public
     * @static
     */
    public static function main() {
        require_once "PHPUnit2/TextUI/TestRunner.php";

        $suite  = new PHPUnit2_Framework_TestSuite("GdkPixbufAnimationIterTest");
        $result = PHPUnit2_TextUI_TestRunner::run($suite);
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

    /**
     * @todo Implement testGet_delay_time().
     */
    public function testGet_delay_time() {
        // Remove the following line when you implement this test.
        throw new PHPUnit2_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testGet_pixbuf().
     */
    public function testGet_pixbuf() {
        // Remove the following line when you implement this test.
        throw new PHPUnit2_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testOn_currently_loading_frame().
     */
    public function testOn_currently_loading_frame() {
        // Remove the following line when you implement this test.
        throw new PHPUnit2_Framework_IncompleteTestError;
    }
}

// Call GdkPixbufAnimationIterTest::main() if this source file is executed directly.
if (PHPUnit2_MAIN_METHOD == "GdkPixbufAnimationIterTest::main") {
    GdkPixbufAnimationIterTest::main();
}
?>
