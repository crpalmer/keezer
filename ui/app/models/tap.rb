class Tap < ActiveRecord::Base
  has_one :beer

  def beer
    Beer.find(beer_id)
  end
end
