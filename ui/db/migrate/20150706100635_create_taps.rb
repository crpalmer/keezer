class CreateTaps < ActiveRecord::Migration
  def change
    create_table :taps do |t|
      t.integer :tap_number
      t.timestamps null: false
      t.references :beer
    end
  end
end
