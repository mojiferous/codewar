<?php
/**
 * code_bot.php
 * defines the codebot class
 * 12/20/12 Mojiferous
 */
class code_bot {

  //many of these variables are public so that they store correctly in a $_SESSION variable
  private $body_parts = array(); //< the raw code array
  public $code_array = array(); //< the compiled code array
  public $line_num; //< integer value of current line number
  public $call_stack = array(); //< array of line numbers that the code calls back to, used for gosub commands
  private $command_array = array(
    'goto',
    'gosub',
    'if',
    'endsub',
  ); //< array of language commands

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

      $ret_val = array();

      $zero_counter = 0;

      do {
        //skip over empty lines, which the compiler spits out to allow for proper error checking

        //reset the counter back to 0
        if($this->line_num >= count($this->code_array)) {
          $this->line_num = 0;
          $zero_counter++;

          if($zero_counter > 1) {
            //we've gone through this code more than once, set and error and break the while loop
            _set_array_error($ret_val, 'Your code is nothing but a bunch of empty lines -- existentialism doesn\'t really work here.');
            break;
          }
        }

        $line_val = $this->code_array[$this->line_num];
        $this->line_num++;

      } while (empty($line_val));

      $ret_val['line_num'] = $this->line_num-1;

      dpm(json_encode($line_val), 'this line'); dpm($this->code_array, 'full array');

      if(isset($line_val[0])) {

        switch($line_val[0]) {
          case 'assign':
            //variable assignment
            if(isset($line_val[1]) && isset($line_val[2])) {
              //make sure we have values for the assignment

              //trim blank space
              $var_name = trim($line_val[1]);
              $var_val = trim($line_val[2]);

              if(isset($this->code_array['vars']->$var_name)) {

                if(!is_numeric($var_val)) {
                  //if the value being assigned is not numeric, figure out what is going on here
                  if(!$this->eval_code($var_val)) {
                    //there has been an error!
                    _set_array_error($ret_val, 'Assignment error on line #'.$ret_val['line_num']);
                  }
                }

                $this->code_array['vars']->$var_name = $var_val;

                $ret_val['assigned '.$var_name] = $var_val;

              } else {
                //the variable you are assigning does not exist
                _set_array_error($ret_val, 'Invalid variable assignment on line #'.$ret_val['line_num'].'. Variable '.$var_name.' does not exist');
              }

            } else {
              //something isn't right here
              _set_array_error($ret_val, 'Invalid assignment or code on line #'.$ret_val['line_num']);
            }

            break;
          case 'endsub':
            //end a sub function and jump back to where you were
            if(isset($this->call_stack) && count($this->call_stack) > 0) {
              array_reverse($this->call_stack);

              $this->line_num = array_shift($this->call_stack);
            } else {
              //there is nothing in the call stack
              _set_array_error($ret_val, 'Stack underflow error');
            }

            break;
          case 'goto':
            //jump to new function without return ability

            $func_name = trim($line_val[1]);

            if(isset($line_val[1])) {
              if(isset($this->code_array['funcs']->$func_name)) {
                //the next line will be the head of the function

                $this->line_num = $this->code_array['funcs']->$func_name;

              } else {
                //the function this goto was pointing to does not exist
                _set_array_error($ret_val, 'The function '.$func_name.' on line #'.$ret_val['line_num'].' does not exist');
              }

            } else {
              //there is no goto value, break here
              _set_array_error($ret_val, 'Attempt to goto nothing on line #'.$ret_val['line_num'].'. Very nihilistic.');
            }

            break;
          case 'gosub':
            //jump to sub function

            $func_name = $line_val[1];

            if(isset($line_val[1])) {
              if(isset($this->code_array['funcs']->$func_name)) {
                //the next line will be the head of the function

                //add the next line as the "return"
                $this->call_stack[] = $this->line_num++;

                $this->line_num = $this->code_array['funcs']->$func_name;

              } else {
                //the function this goto was pointing to does not exist
                _set_array_error($ret_val, 'The function '.$func_name.' on line #'.$ret_val['line_num'].' does not exist');
              }

            } else {
              //there is no goto value, break here
              _set_array_error($ret_val, 'Attempt to gosub nothing on line #'.$ret_val['line_num'].'. Very nihilistic.');
            }

            break;
          case 'if':
            //if statement
            break;

          case 'sysfunc':
            //sets system functions
            break;
          default:
            break;
        }

      } elseif ($zero_counter == 0) {
        //for some reason the code here was not parsed correctly, should not happen, but if it does, break here
        _set_array_error($ret_val, 'Unable to parse code at line #'.$this->line_num);
      }

      return $ret_val;
    }

    return array('ERROR' => 'No code!');
  }

  private function eval_code(&$result_var) {
    //replace all our vars
    $vars = (array)$this->code_array['vars'];

    //split this string by mathematical symbols
    $split_arg = preg_split('/[\s\(\),+*\/-]+|[\.]\s+/', $result_var, -1, PREG_SPLIT_NO_EMPTY);
    //split the string into mathematical operators
    $split_op = preg_split('/[a-z\d_]/', $result_var, -1, PREG_SPLIT_NO_EMPTY);

    //loop through the arguments
    foreach ($split_arg as $key => $argument) {
      if(!is_numeric($argument)) {
        if(isset($vars[$argument])) {
          //there is a variable by the name of "argument" here
          $split_arg[$key] = $vars[$argument];
        } else {
          //we don't have a value for this, function should error out
          $result_var = -500;
          return false;
        }
      }
    }

    $final_formula = '';

    //loop through each part and build a final formula
    $arg_count = count($split_arg);
    $op_count = count($split_op);

    $max_count = $arg_count;
    if($op_count > $arg_count) $max_count = $op_count;

    for($n=0; $n<$max_count; $n++) {
      if(!isset($split_op[$n])) $split_op[$n] = '';
      if(!isset($split_arg[$n])) $split_arg[$n] = '';

      if($op_count > $arg_count) {
        //there are more operators than symbols, put them first
        $final_formula .= $split_op[$n].$split_arg[$n];
      } else {
        //there are more symbols than operators
        $final_formula .= $split_arg[$n].$split_op[$n];
      }
    }

    $result_var = eval("return ($final_formula);");

    return true;
  }

  /**
   * parses the body code into an array, which is then json encoded by the build function
   */
  public function parse_code() {
    $line_num = 0;
    $this->set_system_vars();
    $this->code_array['funcs'] = array();

    do {
      $this_command = $this->return_next_value();

      //every line should have a value, even if empty -- the run should just ignore count() == 0 elements
      //this allows our errors to correspond to an actual line in the code and not a compiled line
      $this->code_array[$line_num] = array();

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

            if(substr($this_command,0,3) == 'dim') {
              //this is a variable init
              $var_name = trim(str_ireplace('dim', '', $this_command));

              $this->code_array['vars'][$var_name] = 0;


            } else {
              //explode the line by spaces and switch based on
              $words = explode(' ', $this_command);

              if(in_array($words[0], $this->command_array)) {
                //these are commands, make the line an array

                //search for commands within parenthesis, which should only occur in IF statements
                if($words[0] == 'if' && preg_match_all("/(\\(.*\\))/is", $this_command, $matches)) {
                  if(isset($matches[1])) {
                    $orig = $matches[1];
                    $new_parse = str_ireplace(' ', '', $orig);

                    $this_command = str_ireplace($orig, $new_parse, $this_command);

                    $words = explode(' ', $this_command);
                  }
                }

                $this->code_array[$line_num] = $words;
              } else {

                if(stripos($this_command, '=') !== false) {
                  //this is an assignment, or at least should be
                  $words = explode('=', $this_command);

                  foreach ($words as $key => $word) {
                    $words[$key] = trim($word);
                  }


                  array_unshift($words, 'assign');

                  $this->code_array[$line_num] = $words;
                } else {

                  //this should be a system function, like shoot or aim
                  if (preg_match_all("/(.*?)(\\[.*?\\])/is", $this_command, $matches) ) {
                    if(count($matches) > 1 && isset($matches[1][0]) && isset($matches[2][0])) {
                      //set values for the function call from the regex result
                      $this->code_array[$line_num] = array('sysfunc', $matches[1][0], str_ireplace('[', '', str_ireplace(']', '', $matches[2][0])));


                    }

                  }

                }

              }


            }


          }

        }

      }

      $line_num++;

    } while (count($this->body_parts) > 0);
  }

  /**
   * set system variables and functions
   */
  private function set_system_vars() {
    //system variables
    $this->code_array['vars']['_x'] = 0;
    $this->code_array['vars']['_y'] = 0;
    $this->code_array['vars']['_aim'] = 0;

    //system functions, also set with assignment -- these are similar to above, but either have a result or return a value,
    $this->code_array['system_funcs']['_shoot'] = 0;
    $this->code_array['system_funcs']['_look'] = 0;
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
      return strtolower(trim(array_shift($this->body_parts)));
    }

    return '';
  }

}

/**
 * simple helper function to set an error value in the return array when running code
 * @param $val_array
 * @param $error_code
 */
function _set_array_error(&$val_array, $error_code) {
  $val_array['ERROR'] = $error_code;
}
