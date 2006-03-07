<?php
// Call GtkDialogTest::main() if this source file is executed directly.
if (!defined("PHPUnit2_MAIN_METHOD")) {
    define("PHPUnit2_MAIN_METHOD", "GtkDialogTest::main");
}

require_once "PHPUnit2/Framework/TestCase.php";
require_once "PHPUnit2/Framework/TestSuite.php";

// You may remove the following line when all tests have been implemented.
require_once "PHPUnit2/Framework/IncompleteTestError.php";



/**
 * Test class for GtkDialog.
 * Generated by PHPUnit2_Util_Skeleton on 2006-03-07 at 13:26:41.
 */
class GtkDialogTest extends PHPUnit2_Framework_TestCase {
    /**
     * Runs the test methods of this class.
     *
     * @access public
     * @static
     */
    public static function main() {
        require_once "PHPUnit2/TextUI/TestRunner.php";

        $suite  = new PHPUnit2_Framework_TestSuite("GtkDialogTest");
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
     * @todo Implement testAdd_action_widget().
     */
    public function testAdd_action_widget() {
        // Remove the following line when you implement this test.
        throw new PHPUnit2_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testAdd_button().
     */
    public function testAdd_button() {
        // Remove the following line when you implement this test.
        throw new PHPUnit2_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testAdd_buttons().
     */
    public function testAdd_buttons() {
        // Remove the following line when you implement this test.
        throw new PHPUnit2_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testGet_has_separator().
     */
    public function testGet_has_separator() {
        // Remove the following line when you implement this test.
        throw new PHPUnit2_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testResponse().
     */
    public function testResponse() {
        // Remove the following line when you implement this test.
        throw new PHPUnit2_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testRun().
     */
    public function testRun() {
        // Remove the following line when you implement this test.
        throw new PHPUnit2_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testSet_default_response().
     */
    public function testSet_default_response() {
        // Remove the following line when you implement this test.
        throw new PHPUnit2_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testSet_has_separator().
     */
    public function testSet_has_separator() {
        // Remove the following line when you implement this test.
        throw new PHPUnit2_Framework_IncompleteTestError;
    }

    /**
     * @todo Implement testSet_response_sensitive().
     */
    public function testSet_response_sensitive() {
        // Remove the following line when you implement this test.
        throw new PHPUnit2_Framework_IncompleteTestError;
    }
}

// Call GtkDialogTest::main() if this source file is executed directly.
if (PHPUnit2_MAIN_METHOD == "GtkDialogTest::main") {
    GtkDialogTest::main();
}
?>
