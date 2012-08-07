<?php
// $Id$
/**
* @file
* Brazil callback test for Global World Solutions (GWS)
*/

/**
 * Implementation of hook_voipscript_get_script_names()
 */
function gws_voipscript_get_script_names() {
  $script_names[] = 'gws_answer_script';
  return $script_names;
}

/**
 * Implementation of hook_voipscript_load_script()
 */
function gws_voipscript_load_script($script_name, $options=NULL) {
  $script = NULL;
  switch ($script_name) {
    default:
      break;
    case 'gws_answer_script':
watchdog('debug', 'in: gws_answer_script');
      $callback_script_name = 'gws_survey_script';
      $script = new VoipScript('gws_answer_script');
      $script->addSet('callback_script_name', $callback_script_name);
      $script->addSet('gws_callback_result', "^_voipscript_set_callback(%cid)");
      $script->addReject();
      break;
    case 'voipscript_exec_callback_script':  
watchdog('debug', 'in: voipscript_exec_callback_script');
      $script = new VoipScript('voipscript_exec_callback_script');
      $script->addSet('voipscript_callback_result', '^_voipscript_exec_callback(%caller_number, %callback_script_name)');
      break;
    case 'gws_survey_script':
watchdog('debug', 'in: gws_survey_script');
      $script = new VoipScript('gws_survey_script');
      $script->addSay(v("Welcome to Test Script. Please choose between 0 to 9 in the following questions."));
      
      //Question1
      $script->addLabel('question1');
      $prompt = v("Question 1. ");
      $timeout = 5;
      $end_key = '';
      $num_digits = 1;
      $script->addGetInput($prompt, $num_digits, $end_key, $timeout);
      
      //Continue if user answered
      $script->addGotoIf('answer1_save', "^%input_digits != '' && %input_digits != '*' && %input_digits != '#'");
      //Return if no input:
      $script->addSay(v('Invalid input received. Please try again.'));
      $script->addGoto('question1');
      
      $script->addLabel('answer1_save');
      $script->addSet('answer1', "%input_digits");
      
      //Question2
      $script->addLabel('question2');
      $prompt = v("Question 2. ");
      $script->addGetInput($prompt, $num_digits, $end_key, $timeout);
      //Continue if user answered
      $script->addGotoIf('answer2_save', "^%input_digits != '' && %input_digits != '*' && %input_digits != '#'");
      //Return if no input:
      $script->addSay(v('Invalid input received. Please try again.'));
      $script->addGoto('question2');
      
      $script->addLabel('answer2_save');
      $script->addSet('answer2', "%input_digits");
      
      //Question3
      $script->addLabel('question3');
      $prompt = v("Question 3. ");
      $script->addGetInput($prompt, $num_digits, $end_key, $timeout);
      //Continue if user answered
      $script->addGotoIf('answer3_save', "^%input_digits != '' && %input_digits != '*' && %input_digits != '#'");
      //Return if no input:
      $script->addSay(v('Invalid input received. Please try again.'));
      $script->addGoto('question3');
      
      $script->addLabel('answer3_save');
      $script->addSet('answer3', "%input_digits");
      
      //Question4
      $script->addLabel('question4');
      $prompt = v("Question 4. ");
      $script->addGetInput($prompt, $num_digits, $end_key, $timeout);
      //Continue if user answered
      $script->addGotoIf('answer4_save', "^%input_digits != '' && %input_digits != '*' && %input_digits != '#'");
      //Return if no input:
      $script->addSay(v('Invalid input received. Please try again.'));
      $script->addGoto('question4');
      
      $script->addLabel('answer4_save');
      $script->addSet('answer4', "%input_digits");
      
      //Store the answer values in DB
      $script->addSet('braziltest_save','^_gws_save(%call_id, %dest_number, %answer1, %answer2, %answer3, %answer4)');
      $script->addSay(v('Your results have been saved. Thank you and goodbye.'));
      $script->addHangup();
      break;    
      
  }

  return $script;
}

function _voipscript_set_callback($cid) {
watchdog('debug', "in:  _set_callback($cid)");
  $call = VoipCall::load($cid);
  $call->setHangupScriptName('voipscript_exec_callback_script');
  $call->save();
}

function _voipscript_exec_callback($caller_number, $callback_script_name) {
  watchdog('debug', "in: _exec_callback($caller_number, $callback_script_name)");
  // wait a little for the caller to hang up
  sleep(10);
  // create the new call
  $call = new VoipCall();
  $call->setDestNumber($caller_number);
  $callback_script = VoipScript::loadScript($callback_script_name, $vars);
  $call->setScript($callback_script);
  $call->save();
  // Execute the callback
  voip_dial($call);
}

function _gws_save($call_id, $caller_number, $answer1, $answer2, $answer3, $answer4) {
  $answers = array(
    'answer1' => $answer1,
    'answer2' => $answer2,
    'answer3' => $answer3,
    'answer4' => $answer4,
  );
  $res = db_query("INSERT INTO {gws} (call_id, caller_number, answers, timestamp) VALUES('%s', '%s', '%s', %d)", 
  $call_id, $caller_number, serialize($answers), time());
  return $res;
}

