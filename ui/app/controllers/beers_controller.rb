class BeersController < ApplicationController
  def new
    @beer = Beer.new
    @beer.save
    tap_id = params[:tap_id]
    if tap_id != nil
       redirect_to edit_beer_for_tap_path(id: @beer.id.to_s, tap_id: params[:tap_id])
    else
       render 'edit'
    end
  end

  def edit
    @beer = Beer.find(params[:id])
  end

  def update
    @beer = Beer.find(params[:id])
    if @beer.update(beer_params)
      redirect_after_update
    else
      render 'edit'
    end
  end

  def delete
    @beer = Beer.find(params[:id])
    @beer.destroy
    redirect_after_update
  end

  private
    def beer_params
      params.require(:beer).permit(:name, :brewer, :style, :ibu, :srm, :abv)
    end

    def redirect_after_update
      tap_id = params[:tap_id]
      if tap_id != nil
	redirect_to edit_tap_path(tap_id)
      else
        redirect_to root_path
      end
    end
end
