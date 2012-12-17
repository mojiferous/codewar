<?php

$btth = array(
    'breadcrumb' => 1,
    'title_pref' => 1,
    'title' => 1,
    'title_suf' => 1,
    'messages' => 1,
    'tabs' => 1,
    'help' => 1,
    'action_links' => 1,
);

?>

<?php include('includes/header.inc'); ?> 

<?php include('includes/btth.inc'); ?> 

<div id="content" class="column" role="main">
    
    <?php print render($page['highlighted']); ?>
    
    <?php print render($page['content']); ?>
    
    <?php print $feed_icons; ?>
    
</div><!-- /#content -->

<?php print render($page['sidebar_first']); ?>

<?php print render($page['sidebar_second']); ?>
    
<?php include('includes/footer.inc'); ?>