<?php
// Call GdkPixbufAnimationIterTest::main() if this source file is executed directly.
if (!defined("PHPUnit_MAIN_METHOD")) {
    define("PHPUnit_MAIN_METHOD", "GdkPixbufAnimationIterTest::main");
}

require_once "PHPUnit/Framework/TestCase.php";
require_once "PHPUnit/Framework/TestSuite.php";

/**
 * Test class for GdkPixbufAnimationIter.
 * Generated by PHPUnit_Util_Skeleton on 2007-02-06 at 21:44:02.
 */
class GdkPixbufAnimationIterTest extends PHPUnit_Framework_TestCase {
    /**
     * Runs the test methods of this class.
     *
     * @access public
     * @static
     */
    public static function main() {
        require_once "PHPUnit/TextUI/TestRunner.php";

        $suite  = new PHPUnit_Framework_TestSuite("GdkPixbufAnimationIterTest");
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

    /**
     * @todo Implement testGet_delay_time().
     */
    public function testGet_delay_time() {
        // Remove the following line when you implement this test.
        $this->markTestIncomplete(
          "This test has not been implemented yet."
        );
    }

    /**
     * @todo Implement testGet_pixbuf().
     */
    public function testGet_pixbuf() {
        // Remove the following line when you implement this test.
        $this->markTestIncomplete(
          "This test has not been implemented yet."
        );
    }

    /**
     * @todo Implement testOn_currently_loading_frame().
     */
    public function testOn_currently_loading_frame() {
        // Remove the following line when you implement this test.
        $this->markTestIncomplete(
          "This test has not been implemented yet."
        );
    }
}

// Call GdkPixbufAnimationIterTest::main() if this source file is executed directly.
if (PHPUnit_MAIN_METHOD == "GdkPixbufAnimationIterTest::main") {
    GdkPixbufAnimationIterTest::main();
}
?>
