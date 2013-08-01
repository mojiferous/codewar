<?php
/**
 * code_bot.php
 * defines the codebot class
 * 12/20/12 Mojiferous
 */
class code_bot {

  private $body_parts = array(); //< the raw code array
  public $code_array = array(); //< the compiled code array
  public $line_num; //< integer value of current line number

  public function __construct($raw_code) {
    $this->body_parts = $raw_code;
    $this->line_num = 0;
  }

  /**
   * run the bots code
   */
  public function run_code() {
    if(isset($this->code_array) && count($this->code_array) > 0) {
      //we actually have something here

      $this->line_num++;
      if($this->line_num >= count($this->code_array)) {
        $this->line_num = 0;
      }

      return $this->code_array[$this->line_num];
    }

    return '';
  }

  /**
   * parses the body code into an array, which is then json encoded by the build function
   */
  public function parse_code() {
    $line_num = 0;
    do {
      $this_command = $this->return_next_value();

      if(!empty($this_command)) {

        $first_char = substr($this_command,0,1);
        $double_first = substr($this_command,0,2);
        $last_char = substr($this_command, -1, 1);

        if(($first_char != ';' && $double_first != '//')) {
          //this is not a comment
          if($last_char == ':') {
            //this is a function

            $func_name = substr($this_command, 0, strlen($this_command)-1);

            $this->code_array['funcs'][$func_name] = $line_num;

          } else {
            $this->code_array[$line_num] = $this_command;

            $line_num++;
          }

        }


      }

    } while (count($this->body_parts) > 0);
  }

  /**
   * adds an error to the code array
   * @param string $error_string
   */
  private function add_error($error_string) {
    $this->code_array['errors'][] = $error_string.' on line #'.$this->line_num;
  }

  /**
   * checks for stack underflow errors
   * @return bool
   */
  private function check_array_length() {
    if(count($this->body_parts) > 0) {
      return true;
    }

    return false;
  }

  /**
   * return the first (next) value of the code array
   * @return string
   */
  private function return_next_value() {
    if($this->check_array_length()) {
      return trim(array_shift($this->body_parts));
    }

    return '';
  }

}
