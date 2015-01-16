require "rake/extensiontask"

Rake::ExtensionTask.new "rsonal" do |ext|
  ext.lib_dir = "lib/rsonal"
end

task "test" => [:compile] do |t|
  sh "ruby -Ilib test/test_suite.rb"
end