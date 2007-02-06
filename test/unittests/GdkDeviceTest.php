<?php
// Call GdkDeviceTest::main() if this source file is executed directly.
if (!defined("PHPUnit_MAIN_METHOD")) {
    define("PHPUnit_MAIN_METHOD", "GdkDeviceTest::main");
}

require_once "PHPUnit/Framework/TestCase.php";
require_once "PHPUnit/Framework/TestSuite.php";

// You may remove the following line when all tests have been implemented.
require_once "PHPUnit/Framework/IncompleteTestError.php";



/**
 * Test class for GdkDevice.
 * Generated by PHPUnit_Util_Skeleton on 2006-03-07 at 13:26:40.
 */
class GdkDeviceTest extends PHPUnit_Framework_TestCase {
    /**
     * Runs the test methods of this class.
     *
     * @access public
     * @static
     */
    public static function main() {
        require_once "PHPUnit/TextUI/TestRunner.php";

        $suite  = new PHPUnit_Framework_TestSuite("GdkDeviceTest");
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
     * @todo Implement testSet_axis_use().
     */
    public function testSet_axis_use() {
        // Remove the following line when you implement this test.
        throw new PHPUnit_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testSet_key().
     */
    public function testSet_key() {
        // Remove the following line when you implement this test.
        throw new PHPUnit_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testSet_mode().
     */
    public function testSet_mode() {
        // Remove the following line when you implement this test.
        throw new PHPUnit_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testSet_source().
     */
    public function testSet_source() {
        // Remove the following line when you implement this test.
        throw new PHPUnit_Framework_IncompleteTestError;
    }
}

// Call GdkDeviceTest::main() if this source file is executed directly.
if (PHPUnit_MAIN_METHOD == "GdkDeviceTest::main") {
    GdkDeviceTest::main();
}
?>
