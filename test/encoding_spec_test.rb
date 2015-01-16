require 'minitest/spec'
require 'minitest/autorun'
require 'rsonal'

describe Rsonal do
  it "should respect UTF-8 literals" do
    assert_equal "\"\\u1234\"", Rsonal.write_json("\u1234")
  end

  it "should handle larger amounts of UTF-8 literals" do
    assert_equal '"\u0311\u8844\u7733\u7581\u2211"',
      Rsonal.write_json("\u0311\u8844\u7733\u7581\u2211")
  end

  it "should allow mixing of normal characters and unicode chars" do
    json = Rsonal.write_json "H\u3321llo\u6712world\u1100"
    assert_equal '"H\u3321llo\u6712world\u1100"', json
  end
end