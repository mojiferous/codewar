# =============================================================================
# DEPLOYMENT RECIPE FOR DRUPAL SITES
# =============================================================================

# This recipe is for the automated deployment and environment configuration
# of Drupal sites in testing and production environments

# =========================================================================
# Init multistage
# =========================================================================

require 'capistrano/ext/multistage'
set :stages, %w(develop staging production)
set :default_stage, "develop"
set :deploy_subdir, "site"

# =========================================================================
# Set server configuration
# =========================================================================

set :use_sudo, false     

default_run_options[:pty] = true

# number of releases to keep
set :keep_releases, 8

# =========================================================================
# Deploy tasks
# =========================================================================
 
namespace :deploy do
# derive site paths from site configuration
task :set_paths do
    
    logger.info "Setting configuration for #{application} #{stage}"
    set :current_path, "#{deploy_to}/current"
    set :shared_path, "#{deploy_to}/shared"

    set :shared_default_site_path, "#{shared_path}/default"
    set :shared_default_site_files_path, "#{shared_default_site_path}/files"
    set :shared_default_site_private_path, "#{shared_path}/private"
    
  end

  before "deploy:setup", "deploy:set_paths"  
  before "deploy", "deploy:set_paths"
  before "deploy", "deploy:production_verify"
  before "deploy", "deploy:set_git_config"
  before "deploy", "deploy:releases_dir_check"
  after "deploy", "deploy:create_site_structure"
  after "deploy", "deploy:create_data_symlinks"
  after "deploy", "deploy:create_env_robots"
  after "deploy", "deploy:cleanup"
  after "deploy", "deploy:upload_local_files"
  
  desc "Set up the expected application directory structure on all boxes"
  task :setup, :except => { :no_release => true } do
    logger.info "Creating initial site folder structure."
    sudo("rm -rf #{deploy_to}/test")
    sudo("mkdir -p #{deploy_to} #{releases_path} #{shared_path} #{shared_default_site_path} #{shared_default_site_files_path} #{shared_default_site_private_path}")
    sudo("chown #{file_owner_user} #{deploy_to} #{releases_path} #{shared_path} #{shared_default_site_path} #{shared_default_site_files_path} #{shared_default_site_private_path}") 
    # give this user and apache write access to the shared
    sudo("chown -R #{file_owner_user}:#{web_server_user} -R #{shared_default_site_files_path}")
    sudo("sudo chmod 775 -R #{shared_default_site_files_path}") 
    # give this user and apache write access to the shared
    sudo("chown -R #{file_owner_user}:#{web_server_user} -R #{shared_default_site_private_path}")
    sudo("sudo chmod 775 -R #{shared_default_site_private_path}")
    logger.info "Creating .ssh folder for password-less access"
    sudo("mkdir -p #{site_root}/.ssh")
    sudo("chown #{file_owner_user} #{site_root}/.ssh")    
    sudo("chmod 700 -R #{site_root}/.ssh")    
  end

  task :launch do
    logger.info "Creating symlink to site running in #{current_path}"
    # link names that we don't want to clobber
    httpdirs = ["#{site_root}/httpdocs", "#{site_root}/httpsdocs"]
    for httpdir in httpdirs
      # see if file already exists on server
      set :httpdir_exist, capture("find #{site_root} -path #{httpdir} | wc -l").chomp!
      # if the file exists
      if ("#{httpdir_exist}" != "0")
        logger.info "File/directory already exists. Backing up..."
        # backup the file/dir then create symlink
        sudo("mv #{httpdir} #{httpdir}_old_$(date +%Y%m%d%H%M%S) && ln -sf #{current_path} #{httpdir}")
      else
        # nothing there, so just make the symlink to current
        sudo("ln -sf #{current_path} #{httpdir}")
      end
    end
  end
  
  # make sure nothing weird got created in releases directory. 
  # proceeding otherwise could break the site
  task :releases_dir_check do 
    # check releases dir for anything that doesn't start with 20
    set :unauthorized_dirs, capture("ls -1 #{releases_path} | grep -v ^20 | wc -l").chomp!
    # if any bad directories are detected, abort deployment
    if ("#{unauthorized_dirs}" != "0")
      logger.info "Ew, your releases directory isn't clean. You should be ashamed."
      error = CommandError.new("Aborting deployment")      
      raise error      
    end
  end

  # derive git set up from site configuration
  task :production_verify do 
    # if production environment, confirm deployment.
    if ("#{stage}" == "production")
      production_confirmation = Capistrano::CLI.ui.ask("Confirm deployment to production(y/N):")
      if !(production_confirmation.downcase == "y" || production_confirmation.downcase == "yes")
        error = CommandError.new("Aborting deployment")      
        raise error      
      end
    end
  end
  # derive git set up from site configuration
  task :set_git_config do    
    set :scm, "git"  
    set :scm_verbose, true            
    ssh_options[:forward_agent] = true
    set :repository,  "git@github.com:elevatedthird/#{application}.git"  
    set :deploy_via, :remote_cache
  end
  
  task :update do
    transaction do
      update_code
      symlink
    end
  end
	
  task :symlink do
    transaction do
        run "ln -nfs #{current_release} #{deploy_to}/#{current_dir}"
        # run "rm -Rf #{document_root}"
        # run "ln -nfs #{deploy_to}/#{current_dir} #{document_root}"
    end
  end  
  
  task :finalize_update do
    transaction do
    end
  end                                   
  
  task :create_site_structure do 
    transaction do    
      # move stage specific settings.php into default folder
      run "if [ -f #{current_path}/sites/default/#{stage}.settings.php ]; then mv -f #{current_path}/sites/default/#{stage}.settings.php #{shared_default_site_path}/settings.php; fi"
      # rename <stage>.htaccess to default .htaccess if existent
      run "if [ -f #{current_path}/#{stage}.htaccess ]; then mv -f #{current_path}/#{stage}.htaccess #{current_path}/.htaccess; fi"
    end
  end 
  
  task :create_data_symlinks do
    transaction do
      run "rm -rf #{current_path}/sites/default"
      run "ln -nfs #{shared_path}/default #{current_path}/sites"
    end
  end                              
  
  task :create_env_robots do
    transaction do  
      if !("#{stage}" == "production")
        run "echo -e 'User-agent: * \\nDisallow: \/' > #{current_path}/robots.txt"
      end
    end
  end                               
  
  task :upload_local_files do
    transaction do  
      system 'if [ "$(ls -A upload/files)" ]; then echo "not empty"; fi'
    end
  end 
  
 
  # Each of the following tasks are Rails specific. They're removed.
  task :migrate do
  end
 
  task :migrations do
  end
 
  task :cold do
  end
 
  task :start do
  end
 
  task :stop do
  end
 
  task :restart do
  end
  

end 
