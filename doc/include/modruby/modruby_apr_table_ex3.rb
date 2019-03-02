table = APR::Table.new(APR::Pool.new())

# Dumps the contents of a Table
def dump(t)
  puts 'Dumping Table:'
  t.each do |key, value|
    puts "  #{key}=#{value}"
  end
end

table.add('Set-Cookie', 'lp=13ds4; Domain=.launchpad.net; Path=/')
table.add('Set-Cookie', 'edge=453fd; Domain=.launchpad.net; Path=/')
dump(table)

table.merge('Set-Cookie', 'ISO-8859-1')
table.merge('Set-Cookie', 'utf8')
dump(table)

table.set('Set-Cookie', 'wiped out')
dump(table)
