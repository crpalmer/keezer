class CreateBeers < ActiveRecord::Migration
  def change
    create_table :beers do |t|
      t.string :name
      t.string :brewer
      t.string :style
      t.decimal :ibu
      t.decimal :srm
      t.decimal :abv

      t.timestamps null: false
    end
  end
end
