<?php
// $Id: ds-2col-stacked.tpl.php,v 1.1.2.8 2011/02/01 13:47:47 jyve Exp $

/**
 * @file
 * Display Suite 2 column stacked template.
 */
?>
<div class="<?php print $classes;?> clearfix">

  <?php if (isset($title_suffix['contextual_links'])): ?>
  <?php print render($title_suffix['contextual_links']); ?>
  <?php endif; ?>

  <?php if ($header): ?>
    <div class="ds-group group-header<?php print $header_classes; ?>"><div class="ds-group-inner">
      <?php print $header; ?>
    </div> </div>
  <?php endif; ?>
  <div class="group-content-col-wrapper">
  <?php if ($left): ?>
    <div class="ds-group group-left<?php print $left_classes; ?>"><div class="ds-group-inner">
      <?php print $left; ?>
    </div></div>
  <?php endif; ?>

  <?php if ($right): ?>
    <div class="ds-group group-right<?php print $right_classes; ?>"><div class="ds-group-inner">
      <?php print $right; ?>
    </div></div>
  <?php endif; ?>
  </div>
  <?php if ($footer): ?>
    <div class="ds-group group-footer<?php print $footer_classes; ?>"><div class="ds-group-inner">
      <?php print $footer; ?>
    </div></div>
  <?php endif; ?>
</div>
