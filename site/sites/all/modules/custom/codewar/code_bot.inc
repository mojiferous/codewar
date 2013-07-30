<?php
/**
 * code_bot.php
 * defines the codebot class
 * 12/20/12 Mojiferous
 */
class code_bot {

  private $body_parts = array(); //< the raw code array
  public $code_array = array(); //< the compiled code array
  private $this_func = ''; //<the current function name
  private $if_array = array(); //< the array of ifs
  private $line_num; //< integer value of current line number

  public function __construct($raw_code) {
    $this->body_parts = $raw_code;
  }

  public function parse_code() {
    $this->line_num = 0;
    do {
      $this_command = $this->return_next_value();

      switch($this_command) {
        case 'dim':
          $this->line_num++;
          if($this->check_array_length()) {
            $this->code_array['vars'][$this->return_next_value()] = 0;
          }
          break;

        case 'begn':
          $this->line_num++;
          if($this->check_array_length()) {
            if($this->check_for_not_function()) {
              $this->this_func = $this->return_next_value();
            }
          }
          break;

        case 'if':
          $this->line_num++;
          if($this->check_array_length() && $this->check_for_function()) {
            $this->code_array[$this->this_func][] = 'IF '.$this->return_next_value();
            $this->if_array[] = array(0);
          }
          break;

        case 'end':
          $this->line_num++;
          if(count($this->if_array) > 0) {
            //we're in an if
            $if_val = $this->if_array[count($this->if_array)-1];
            array_pop($this->if_array);
            if(count($this->if_array) > 0) {
              //we're in a nested if
              $this->code_array[$this->this_func][] = 'END IF';
            } elseif($this->check_for_function()) {
              $this->code_array[$this->this_func][] = 'END IF';
            }
          } else {
            //we're at the end of the function
            $this->this_func = '';
          }
          break;

        case '':
          //empty line
          $this->line_num++;
          break;

        default:
          $this->line_num++;
          if($this->check_array_length() && $this->check_for_function()) {
            $this->code_array[$this->this_func][] = $this_command.' '.$this->return_next_value();
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

    $this->add_error('Stack underflow');
    return false;
  }

  /**
   * checks for improperly nested functions
   * @return bool
   */
  private function check_for_not_function() {
    if($this->this_func != '') {
      //we're still in a function
      $this->add_error('Function '.$this->this_func.' missing end statement');
      $blank = $this->return_next_value();
      return false;
    }

    return true;
  }

  /**
   * checks to make sure we're in a function
   * @return bool
   */
  private function check_for_function() {
    if($this->this_func == '') {
      //we aren't in a function, code doesn't belong here
      $this->add_error('Errant code not in function');
      //clear out the next value
      $blank = $this->return_next_value();
      return false;
    }

    return true;
  }

  /**
   * return the first (next) value of the code array
   * @return string
   */
  private function return_next_value() {
    if($this->check_array_length()) {
      return array_shift($this->body_parts);
    }

    return '';
  }

}
