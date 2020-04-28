class TapsController < ApplicationController
  def new
    @tap = Tap.new
    taps = ordered_taps
    @tap.tap_number = taps.length+1
    @tap.save
    redirect_to root_path
  end

  def delete_last
    @tap = ordered_taps.last
    @tap.destroy
    redirect_to root_path
  end

  def edit
    @tap = Tap.find(params[:id]);
    @beers = Beer.order("name").all
  end

  def assign
    @tap = Tap.find(params[:id])
    @beer = Beer.find(params[:beer_id])
    @tap.beer_id = @beer.id
    if @tap.save then
	redirect_to root_path
    else
	render edit_path
    end
  end

  def unassign
    @tap = Tap.find(params[:id])
    @tap.beer_id = nil
    @tap.save
    redirect_to root_path
  end

private
    def ordered_taps
      Tap.order("tap_number").all
    end
end
