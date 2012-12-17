# -----------------------------------------------------------------------------
# Site Configuration
# -----------------------------------------------------------------------------

logger.info "Setting Production/Testing/Whatever Environment Configuration"

# Username and password for account access (including shell access)
set :user, "root"

# The file ownership user is the user that should ultimately own the files
set :file_owner_user, "root"

# The web server user will be granted write access to default/files and private
# On CentOS, this is usually "apache", and on Debian this is usually "www-data"
set :web_server_user, "apache"

set :branch, "master" 

set :application, '<site_name>'     
set :site_url, '<example.com>'

# server IP
server "<ip>", :app, :web, :db, :primary => true

# usual path locations, can be edited here for you server if required
set :site_root, "/var/www/vhosts/#{site_url}"
set :deploy_to, "#{site_root}/application"  
