class Tap < ActiveRecord::Base
  has_one :beer

  def beer
    @beer ||= Beer.find(beer_id)
    rescue
      @beer = Beer.new
  end
end
