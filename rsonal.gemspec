spec = Gem::Specification.new do |s|
  s.name    = "rsonal"
  s.version = "0.1"
  s.summary = "Simple fast JSON writer"
  s.homepage = "https://github.com/mbbh/rsonal"
  s.license = "MIT"
  s.email = "mh@wrongexit.de"
  s.description = <<EOF
rsonal is a simple and efficient JSON writer for Ruby. It is designed
to allow passing of a ruby object to it's only function, 'json_write'
which will return specification conforming JSON with as much speed
as possible.
EOF
  s.author  = "Martin Hauser"

  s.files = Dir.glob("ext/**/*.{c,h,rb}") +
            Dir.glob("lib/**/*.rb")

  s.extensions << "ext/rsonal/extconf.rb"

  s.add_development_dependency "rake-compiler", '~> 0'
end
