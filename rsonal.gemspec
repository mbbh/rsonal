Gem::Specification.new do |s|
  s.name    = "rsonal"
  s.version = "0.0.1"
  s.summary = "Yet Another JSON parser/writer"
  s.author  = "Martin Hauser"

  s.files = Dir.glob("ext/**/*.{c,rb}") +
            Dir.glob("lib/**/*.rb")

  s.extensions << "ext/rsonal/extconf.rb"

  s.add_development_dependency "rake-compiler"
end